#include "ach/core/op/ProfileInstall.hh"

#include "ach/core/network/Download.hh"
#include "ach/core/op/Flow.hh"
#include "ach/core/op/Finder.hh"
#include "ach/core/op/Tools.hh"
#include "ach/core/profile/GameProfile.hh"
#include "ach/sys/Schedule.hh"

#include <httplib.h>
#include <uv.h>
#include <lurl.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <log.hh>

namespace Alicorn
{
namespace Op
{

std::string
fetchVersionManifest()
{
  httplib::Client cli(ACH_MOJANG_VERSION_MANIFEST_HOST);
  auto res = cli.Get(ACH_MOJANG_VERSION_MANIFEST_PATH);
  if(res == nullptr || res->status != 200)
    {
      LOG("Failed to fetch version manifest!");
      return "";
    }
  LOG("Successfully fetched version manifest.");
  return res->body;
}

void
installProfile(Flow *flow, FlowCallback sender)
{
  std::string id = flow->data[AL_FLOWVAR_PROFILEID];
  LOG("Installing profile with required id " << id);
  Sys::runOnWorkerThread([=]() -> void {
    // Fetch the version manifest
    sender(AL_MANIFEST);
    std::string manifest = fetchVersionManifest();
    if(manifest.length() == 0)
      {
        sender(AL_ERR);
        return;
      }

    cJSON *mf = cJSON_Parse(manifest.c_str());

    // Resolve special id
    std::string targetID = id;
    cJSON *latestObj = cJSON_GetObjectItem(mf, "latest");
    if(cJSON_IsObject(latestObj))
      {
        if(id == "latest-release")
          {
            cJSON *latestReleaseObj
                = cJSON_GetObjectItem(latestObj, "release");
            if(cJSON_IsString(latestReleaseObj))
              {
                targetID = std::string(cJSON_GetStringValue(latestReleaseObj));
              }
          }
        else if(id == "latest-snapshot")
          {
            cJSON *latestSnapshotObj
                = cJSON_GetObjectItem(latestObj, "snapshot");
            if(cJSON_IsString(latestSnapshotObj))
              {
                targetID = cJSON_GetStringValue(latestSnapshotObj);
              }
          }
      }

    // Query for the target artifact
    sender(AL_FINDPROF);

    cJSON *versions = cJSON_GetObjectItem(mf, "versions");
    cJSON *profArtifact = nullptr;
    if(cJSON_IsArray(versions))
      {
        int len = cJSON_GetArraySize(versions);
        for(int i = 0; i < len; i++)
          {
            cJSON *currentArtifact = cJSON_GetArrayItem(versions, i);
            if(cJSON_IsObject(currentArtifact))
              {
                cJSON *idObj = cJSON_GetObjectItem(currentArtifact, "id");
                if(cJSON_IsString(idObj))
                  {
                    std::string cid = cJSON_GetStringValue(idObj);
                    if(cid == targetID)
                      {
                        profArtifact = currentArtifact;
                        break;
                      }
                  }
              }
          }
      }

    if(profArtifact == nullptr)
      {
        LOG("Could not find specified id " << id);
        cJSON_Delete(mf);
        sender(AL_ERR);
        return;
      }

    std::string url;
    cJSON *urlObj = cJSON_GetObjectItem(profArtifact, "url");
    if(cJSON_IsString(urlObj))
      {
        url = cJSON_GetStringValue(urlObj);
      }
    auto u = LUrlParser::ParseURL::parseURL(url);
    cJSON_Delete(mf);
    if(!u.isValid())
      {
        LOG("No url available for id " << id);
        sender(AL_ERR);
        return;
      }
    LOG("Found matching version with url " << url);
    // Download profile
    sender(AL_GETPROF);
    httplib::Client cli(u.connectionName_);
    auto res = cli.Get(u.pathName_);
    if(res->status != 200)
      {
        LOG("Could not fetch profile from " << url);
        sender(AL_ERR);
        return;
      }

    std::string profileSrc = res->body;
    cJSON *profileObj = cJSON_Parse(profileSrc.c_str());
    if(!cJSON_IsObject(profileObj))
      {
        LOG("Invalid profile content from " << url);
        cJSON_Delete(profileObj);
        sender(AL_ERR);
        return;
      }
    Profile::VersionProfile profile(profileObj);
    cJSON_Delete(profileObj);

    // A simple check, if passed, possibly it's not corrupted
    if(profile.id != targetID)
      {
        LOG("Invalid profile content from " << url);
        sender(AL_ERR);
        return;
      }

    // Pass data to env
    flow->data[AL_FLOWVAR_PROFILESRC] = profileSrc;

    // Write the profile to temp install path
    std::string fileName = targetID + ".json";
    std::string tempPath = getInstallPath(std::filesystem::path("versions")
                                          / targetID / (targetID + ".json"));
    mkParentDirs(tempPath);

    size_t len = profileSrc.size();
    char *rbuf = new char[len];
    memcpy(rbuf, profileSrc.c_str(), len);
    Sys::runOnUVThread([=]() -> void {
      writeFileAsync(tempPath, rbuf, len, [=](int err) -> void {
        delete[] rbuf;
        if(err < 0)
          {
            LOG("Could not write profile to " << tempPath);
            sender(AL_ERR);
          }
        else
          {
            LOG("Successfully fetched profile with id " << profile.id);
            sender(AL_OK);
          }
      });
    });
  });
}

void
installClient(Flow *flow, FlowCallback sender)
{
  // Start work
  Sys::runOnWorkerThread([=]() -> void {
    sender(AL_GETCLIENT);
    std::string profileSrc = flow->data[AL_FLOWVAR_PROFILESRC];
    if(profileSrc.size() == 0)
      {
        sender(AL_ERR);
        return;
      }
    Profile::VersionProfile profile(profileSrc);

    // Create tasks
    using namespace Network;
    DownloadMeta clientMeta, clientMappingsMeta;
    DownloadPack clientPack;
    auto installPrefix = getInstallPath("versions");
    clientMeta
        = DownloadMeta::mkFromArtifact(profile.clientArtifact, installPrefix);
    clientMappingsMeta = DownloadMeta::mkFromArtifact(
        profile.clientMappingsArtifact, installPrefix);
    clientPack.addTask(clientMeta);
    clientPack.addTask(clientMappingsMeta);
    LOG("Starting download client for " << profile.id);

    clientPack.assignUpdateCallback([=](const DownloadPack *p) -> void {
      auto ps = p->getStat();
      if((ps.completed + ps.failed) == ps.total)
        {
          // All processed
          if(ps.failed == 0)
            {
              LOG("Successfully downloaded client.");
              sender(AL_OK);
            }
          else
            {
              LOG("Failed to download client!");
              sender(AL_ERR);
            }
        }
    });
    // Start download
    Sys::runOnWorkerThread([=]() mutable -> void {
      LOG("Started downloading client.");
      clientPack.commit();
      ALL_DOWNLOADS.push_back(clientPack);
    });
  });
}
}
}