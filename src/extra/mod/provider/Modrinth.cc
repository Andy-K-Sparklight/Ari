#include "ach/extra/mod/provider/Modrinth.hh"

#include "ach/extra/mod/Modburin.hh"
#include "ach/util/Commons.hh"
#include "ach/sys/Storage.hh"
#include <base64.hh>
#include <lurl.hpp>
#include <cJSON.h>
#include <httplib.h>
#include <log.hh>
#include "ach/core/network/Download.hh"
#include "ach/core/platform/Tools.hh"
#include "ach/sys/Schedule.hh"
#include "ach/util/Proc.hh"

namespace Alicorn
{
namespace Extra
{
namespace Mod
{

namespace Modrinth
{

#define ACH_MOD_MR_APIROOT "https://api.modrinth.com/v2/"

static std::string
apiRequest(const std::string rel)
{
  auto url = ACH_MOD_MR_APIROOT + rel;
  auto u = LUrlParser::ParseURL::parseURL(url);
  httplib::Client cli(u.connectionName_);
  auto res = cli.Get(u.pathName_);
  if(res == nullptr || res->status != 200)
    {
      LOG("Invalid response reveived from " << url);
      return "";
    }
  return res->body;
}

static std::string
getIconBin(const std::string &url)
{
  auto u = LUrlParser::ParseURL::parseURL(url);
  httplib::Client cli(u.connectionName_);
  auto res = cli.Get(u.pathName_);
  if(res == nullptr || res->status != 200)
    {
      return "";
    }
  return base64_encode(res->body);
}

static void
arrDump(const cJSON *arr, std::set<std::string> &obj)
{
  auto sz = cJSON_GetArraySize(arr);
  for(int i = 0; i < sz; i++)
    {
      auto c = cJSON_GetArrayItem(arr, i);
      obj.insert(cJSON_GetStringValue(c));
    }
}

bool
getModMeta(const std::string &pid)
{
  LOG("Fetching mod meta for " << pid << " from Modrinth.");
  auto res = apiRequest("project/" + pid);
  cJSON *obj = cJSON_Parse(res.c_str());
  if(!cJSON_IsObject(obj))
    {
      cJSON_Delete(obj);
      return false;
    }
  LOG("Parsing meta response.");
  ModMeta meta;
  meta.bid = Commons::genUUID(pid);
  meta.slug = cJSON_GetStringValue(cJSON_GetObjectItem(obj, "slug"));
  meta.displayName = cJSON_GetStringValue(cJSON_GetObjectItem(obj, "title"));
  meta.desc = cJSON_GetStringValue(cJSON_GetObjectItem(obj, "description"));
  meta.provider = "MR";
  meta.icon
      = getIconBin(cJSON_GetStringValue(cJSON_GetObjectItem(obj, "icon_url")));
  meta.pid = pid;
  cJSON *arr = cJSON_GetObjectItem(obj, "versions");
  if(cJSON_IsArray(arr))
    {
      auto sz = cJSON_GetArraySize(arr);
      for(int i = 0; i < sz; i++)
        {
          auto cur = cJSON_GetArrayItem(arr, i);
          if(cJSON_IsString(cur))
            {
              meta.versions.insert(cJSON_GetStringValue(cur));
            }
        }
    }
  cJSON_Delete(obj);
  LOG("Saving mod meta for " << meta.bid);
  auto target = getBurinBase() / "metas" / meta.bid;
  Platform::mkParentDirs(target);
  Sys::saveKVG(target.string(), { meta.toMap() });
  LOG("Successfully fetched meta as " << meta.bid);
  return true;
}

bool
syncModVersions(const std::string &pid)
{
  LOG("Syncing mod versions for " << pid);
  auto target = getBurinBase() / "metas" / pid;
  auto v = Sys::loadKVG(target.string());
  if(v.size() == 0)
    {
      LOG("Meta for " << pid << " not found, have you installed it?");
      return false;
    }
  ModMeta mt(v[0]);
  std::vector<std::pair<std::string, std::string>> results;
  std::vector<std::string> vers;
  for(auto &vid : mt.versions)
    {
      vers.push_back(vid);
    }
  Sys::runOnWorkerThreadMulti(
      [vers, mt](int ind) -> std::function<void()> {
        auto vid = vers[ind];
        return [vid, mt]() -> void {
          LOG("Resolving version " << vid);
          auto res = apiRequest("version/" + vid);
          cJSON *obj = cJSON_Parse(res.c_str());
          if(cJSON_IsObject(obj))
            {
              ModVersion vs;
              vs.provider = "MR";
              vs.pid = cJSON_GetStringValue(cJSON_GetObjectItem(obj, "id"));
              vs.mid = mt.pid;
              vs.slug = mt.slug;
              vs.displayName
                  = cJSON_GetStringValue(cJSON_GetObjectItem(obj, "name"));
              vs.bid = Commons::genUUID(vs.pid);
              auto files = cJSON_GetObjectItem(obj, "files");
              auto fsz = cJSON_GetArraySize(files);
              for(int i = 0; i < fsz; i++)
                {
                  auto f = cJSON_GetArrayItem(files, i);
                  std::string u
                      = cJSON_GetStringValue(cJSON_GetObjectItem(f, "url"));
                  std::string fn = cJSON_GetStringValue(
                      cJSON_GetObjectItem(f, "filename"));
                  if(u.size() > 0)
                    {
                      vs.urls.insert(u + "+" + fn);
                    }
                }
              arrDump(cJSON_GetObjectItem(obj, "game_versions"),
                      vs.gameVersions);
              arrDump(cJSON_GetObjectItem(obj, "loaders"), vs.loaders);
              auto target = getBurinBase() / "versions" / vs.bid;
              Platform::mkParentDirs(target);
              Sys::saveKVG(target.string(), { vs.toMap() });
              LOG("Saved mod version as " << vs.bid);
            }
          cJSON_Delete(obj);
        };
      },
      vers.size());
  LOG("Resolved versions for " << pid);
  return true;
}

void
dlModVersion(const std::string &v, std::function<void(bool)> cb)
{
  LOG("Downloading mod version " << v);
  auto target = getBurinBase() / "versions" / v;
  auto vec = Sys::loadKVG(target.string());
  if(vec.size() == 0)
    {
      LOG("No version meta found for " << v << ", have you installed it?");
      cb(false);
      return;
    }
  ModVersion mv(vec[0]);
  auto dlTarget = getBurinBase() / "files" / v;
  Platform::mkParentDirs(dlTarget);
  Network::DownloadPack pk;
  for(auto &u : mv.urls)
    {
      auto ux = Commons::splitStr(u, "+");
      if(ux.size() == 2)
        {
          Network::DownloadMeta mt;
          mt.baseURL = ux[0];
          // Assume it always ends with the filename
          mt.path = (dlTarget / ux[1]).string();
          pk.addTask(mt);
        }
    }
  pk.assignUpdateCallback([=](const Network::DownloadPack *pak) -> void {
    auto stat = pak->getStat();
    if(stat.completed + stat.failed == stat.total)
      {
        if(stat.failed == 0)
          {
            LOG("Successfully downloaded mod version for " << v);
            cb(true);
          }
        else
          {
            LOG("Some files failed to download for " << v);
            cb(false);
          }
      }
  });
  pk.commit();
  LOG("Started downloading mod version for " << v);
}

static httplib::Server MODRINTH_RPC;

void
setupModrinthServer()
{
  MODRINTH_RPC.Post("/add",
                    [](const httplib::Request &req, httplib::Response &res) {
                      auto mid = req.body;
                      if(mid.size() > 0)
                        {
                          LOG("Received install mod request: " << mid);
                          if(getModMeta(mid))
                            {
                              res.status = 204;
                            }
                          else
                            {
                              res.status = 501;
                            }
                        }
                      else
                        {
                          res.status = 400;
                        }
                      res.set_header("Access-Control-Allow-Origin", "*");
                      res.set_content("", "text/plain");
                    });
  LOG("Modrinth RPC server is listening.");
  MODRINTH_RPC.listen("127.0.0.1", 46432);
}

static bool MODRINTH_WINDOW_CTL = false;

void
showModrinthWindow(std::function<void()> cb)
{
  if(!MODRINTH_WINDOW_CTL)
    {
      MODRINTH_WINDOW_CTL = true;
      Commons::runCommand(Commons::argv0, { "modrinth" },
                          [cb](std::string, int) -> void {
                            MODRINTH_WINDOW_CTL = false;
                            cb();
                          });
    }
  else
    {
      cb();
    }
}

}
}
}
}