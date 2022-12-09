#include "ach/core/network/Download.hh"

#include <cJSON.h>
#include <filesystem>
#include <algorithm>
#include <limits>
#include "ach/drivers/aria2/Aria2Driver.hh"
#include "ach/sys/Schedule.hh"
#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Network
{

DownloadMeta
DownloadMeta::mkFromArtifact(const Profile::Artifact &artifact,
                             const std::string &pathPrefix)
{
  DownloadMeta m;
  m.baseURL = artifact.url;
  if(artifact.sha1.size() > 0)
    {
      m.hash = "sha-1=" + artifact.sha1;
    }
  m.size = artifact.size;
  m.path = Commons::normalizePath(
      std::filesystem::absolute(std::filesystem::path(pathPrefix)
                                / artifact.path)
          .string());
  return m;
}

DownloadMeta
DownloadMeta::mkFromLibrary(const Profile::Library &lib,
                            const std::string &pathPrefix)
{
  DownloadMeta m;
  m.baseURL = lib.artifact.url;
  if(lib.artifact.sha1.size() > 0)
    {
      m.hash = "sha-1=" + lib.artifact.sha1;
    }
  m.size = lib.artifact.size;
  m.path = Commons::normalizePath(
      std::filesystem::absolute(std::filesystem::path(pathPrefix)
                                / lib.artifact.path)
          .string());
  return m;
}

unsigned int
DownloadPack::addTask(const DownloadMeta &meta)
{
  if(existingPaths.contains(meta.path))
    {
      return std::numeric_limits<unsigned int>::max(); // Prevent dup
    }
  if(meta.baseURL.size() == 0 || meta.path.size() == 0)
    {
      return std::numeric_limits<unsigned int>::max();
    }
  existingPaths.insert(meta.path);
  DownloadProcess p;
  p.hash = meta.hash;
  p.path = meta.path;
  p.urls.push_back(meta.baseURL); // TODO: Mirror applying
  p.totalLength = meta.size;
  procs.push_back(p);
  return procs.size() - 1;
}

void
DownloadPack::commit()
{
  using namespace AlicornDrivers;
  Aria2::Aria2Daemon daemon = Aria2::ARIA2_DAEMON;
  std::list<std::pair<std::string, std::list<cJSON *> > > batchOps;
  // Collect commands
  for(auto &p : procs)
    {
      // Configure args
      std::list<cJSON *> args;
      cJSON *uris = cJSON_CreateArray();
      cJSON *options = cJSON_CreateObject();

      for(auto &u : p.urls)
        {
          cJSON *str = cJSON_CreateString(u.c_str());
          cJSON_AddItemToArray(uris, str); // cJSON will delete
        }
      // TODO: Load config

      // Hash
      if(p.hash.size() > 0)
        {
          cJSON_AddStringToObject(options, "checksum", p.hash.c_str());
        }

      // Path
      auto pt = std::filesystem::path(Commons::normalizePath(
          std::filesystem::absolute(std::filesystem::path(p.path))));
      cJSON_AddStringToObject(options, "dir",
                              pt.parent_path().string().c_str());
      cJSON_AddStringToObject(options, "out", pt.filename().string().c_str());

      // Performance
      cJSON_AddStringToObject(options, "min-split-size",
                              "2M"); // TODO: adjust by source speed
      cJSON_AddStringToObject(options, "split", "8"); // Resonable mostly

      // Summarize and call
      args.push_back(uris);
      args.push_back(options);
      batchOps.push_back({ "addUri", args });
    }
  std::string res = daemon.invokeMulti(batchOps);
  for(auto &b : batchOps)
    {
      for(auto &j : b.second)
        {
          cJSON_Delete(j);
        }
    }

  // Assign GIDs
  cJSON *stat = cJSON_Parse(res.c_str());
  if(cJSON_IsArray(stat))
    {
      int i = 0;
      for(auto &p : procs)
        {
          cJSON *resObj = cJSON_GetArrayItem(stat, i);
          if(cJSON_IsObject(resObj))
            {
              cJSON *gid = cJSON_GetObjectItem(resObj, "result");
              if(cJSON_IsString(gid))
                {
                  p.gid = std::string(cJSON_GetStringValue(gid));
                }
            }
          i++;
        }
    }

  cJSON_Delete(stat);
  return;
}

void
DownloadPack::sync()
{
  using namespace AlicornDrivers;
  Aria2::Aria2Daemon daemon = AlicornDrivers::Aria2::ARIA2_DAEMON;
  std::list<std::pair<std::string, std::list<cJSON *> > > batchOps;
  for(auto &p : procs)
    {
      std::list<cJSON *> args;
      cJSON *str = cJSON_CreateString(p.gid.c_str());
      args.push_back(str);
      batchOps.push_back({ "tellStatus", args });
    }

  std::string res;

  res = daemon.invokeMulti(batchOps);
  if(res.size() == 0)
    {
      return; // Abort
    }

  for(auto &b : batchOps)
    {
      for(auto &a : b.second)
        {
          cJSON_Delete(a);
        }
    }
  cJSON *resObj = cJSON_Parse(res.c_str());
  if(cJSON_IsArray(resObj))
    {
      int i = 0;
      // The response should be in the same order as the request
      for(auto &p : procs)
        {
          cJSON *singleRes = cJSON_GetArrayItem(resObj, i);
          if(cJSON_IsObject(singleRes))
            {
              cJSON *o = cJSON_GetObjectItem(singleRes, "result");
              if(cJSON_IsObject(o))
                {
                  cJSON *downloadSpeed
                      = cJSON_GetObjectItem(o, "downloadSpeed");

                  // In RPC standard this is string
                  if(cJSON_IsString(downloadSpeed))
                    {
                      p.speed = (size_t)std::stoull(
                          cJSON_GetStringValue(downloadSpeed));
                    }

                  cJSON *status = cJSON_GetObjectItem(o, "status");
                  if(cJSON_IsString(status))
                    {
                      std::string stat
                          = std::string(cJSON_GetStringValue(status));
                      p.stat = stat;
                    }

                  cJSON *completedLength
                      = cJSON_GetObjectItem(o, "completedLength");

                  if(cJSON_IsString(completedLength))
                    {
                      p.completedLength = (size_t)std::stoull(
                          cJSON_GetStringValue(completedLength));
                    }

                  cJSON *totalLength = cJSON_GetObjectItem(o, "totalLength");
                  if(cJSON_IsString(totalLength))
                    {
                      p.totalLength = (size_t)std::stoull(
                          cJSON_GetStringValue(totalLength));
                    }
                }
            }
          i++;
        }
    }

  cJSON_Delete(resObj);
  updateFunc(this);
}

DownloadPackStatus
DownloadPack::getStat() const
{
  DownloadPackStatus ps;
  for(auto &p : procs)
    {
      ps.totalSize += p.totalLength;
      if(p.stat == "complete")
        {
          ps.completed++;
          ps.completedSize += p.totalLength;
        }
      else if(p.stat == "error")
        {
          ps.failed++;
          ps.failedSize += p.totalLength;
        }
      else if(p.stat == "active")
        {
          ps.speed += p.speed;
          ps.completedSize += p.completedLength;
        }
    }
  ps.total = procs.size();
  return ps;
}

DownloadProcess
DownloadPack::getTask(unsigned int id) const
{
  return procs[id];
}

void
DownloadPack::assignUpdateCallback(
    std::function<void(const DownloadPack *pack)> cb)
{
  updateFunc = cb;
}

std::vector<DownloadPack> ALL_DOWNLOADS;
static bool downloadsSyncing = false;
static uv_timer_t syncTimer;

void
setupDownloadsSync()
{
  Sys::runOnUVThread([&]() -> void {
    uv_timer_init(uv_default_loop(), &syncTimer);
    uv_timer_start(
        &syncTimer,
        [](uv_timer_t *t) -> void {
          if(downloadsSyncing)
            {
              return;
            }
          Sys::runOnWorkerThread([]() -> void {
            downloadsSyncing = true;
            for(size_t i = 0; i < ALL_DOWNLOADS.size(); i++)
              {
                auto d = ALL_DOWNLOADS[i];
                d.sync();
                auto s = d.getStat();
                if((s.completed + s.failed) == s.total)
                  {
                    ALL_DOWNLOADS.erase(ALL_DOWNLOADS.begin() + i);
                    i--;
                  }
              }
            downloadsSyncing = false;
          });
        },
        0, 500);
    // TODO: stop timer on exit
  });
}

void
stopDownloadsSync()
{
  Sys::runOnUVThread(
      []() -> void { uv_close((uv_handle_t *)&syncTimer, NULL); });
}
}
}