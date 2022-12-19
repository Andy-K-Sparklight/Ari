#include "ach/core/op/AssetsInstall.hh"

#include <httplib.h>
#include <cJSON.h>
#include "ach/core/profile/GameProfile.hh"
#include "ach/core/op/Tools.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/network/Download.hh"
#include <lurl.hpp>
#include <list>
#include <string>
#include <iostream>
#include <filesystem>
#include <log.hh>

namespace Alicorn
{
namespace Op
{

void
installAssetIndex(Flow *flow, FlowCallback cb)
{
  cb(AL_GETASSETINDEX);
  Profile::VersionProfile profile = flow->profile;
  LOG("Getting asset index " << profile.assetIndexArtifact.path);
  auto u = LUrlParser::ParseURL::parseURL(profile.assetIndexArtifact.url);
  httplib::Client cli(u.connectionName_);
  auto res = cli.Get(u.pathName_);
  if(res == nullptr || res->status != 200)
    {
      LOG("Could not download asset index " << profile.assetIndexArtifact.url)
      cb(AL_ERR);
      return;
    }
  std::string assetIndexSrc = res->body;
  if(assetIndexSrc.size() == 0)
    {
      LOG("No valid content for asset index "
          << profile.assetIndexArtifact.url)
      cb(AL_ERR);
      return;
    }
  flow->data[AL_FLOWVAR_ASSETINDEX] = assetIndexSrc;

  // Write the profile to temp install path
  std::string fileName = profile.assetIndexArtifact.path + ".json";
  std::string tempPath
      = getInstallPath(std::filesystem::path("assets") / "indexes" / fileName);

  mkParentDirs(tempPath);

  size_t len = assetIndexSrc.size();
  char *rbuf = new char[len];
  memcpy(rbuf, assetIndexSrc.c_str(), len);
  Sys::runOnUVThread([=]() -> void {
    writeFileAsync(tempPath, rbuf, len, [=](int err) -> void {
      delete[] rbuf;
      if(err < 0)
        {
          LOG("Failed to write asset index file to " << tempPath);
          cb(AL_ERR);
        }
      else
        {
          LOG("Successfully written asset index.");
          cb(AL_OK);
        }
    });
  });
}

void
installAssets(Flow *flow, FlowCallback cb)
{
  cb(AL_GETASSETS);
  std::string assetIndexSrc = flow->data[AL_FLOWVAR_ASSETINDEX];
  if(assetIndexSrc.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  std::list<Profile::Asset> assets = Profile::loadAssetIndex(assetIndexSrc);
  if(assets.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  Network::DownloadPack pak;
  LOG("Getting " << assets.size() << " assets.");
  for(auto &a : assets)
    {
      Network::DownloadMeta meta;
      std::string ind = a.hash.substr(0, 2);
      meta.baseURL
          = std::string(ACH_MOJANG_RESOURCES_HOST) + "/" + ind + "/" + a.hash;
      meta.hash = "sha-1=" + a.hash;
      using namespace std::filesystem;
      meta.path = (path(getInstallPath(path("assets") / "objects")) / path(ind)
                   / a.hash)
                      .string();
      meta.size = a.size;
      pak.addTask(meta);
    }
  pak.assignUpdateCallback([=](const Network::DownloadPack *p) -> void {
    auto ps = p->getStat();
    if((ps.completed + ps.failed) == ps.total)
      {
        // All processed
        if(ps.failed == 0)
          {
            LOG("Successfully got all assets.");
            cb(AL_OK);
          }
        else
          {
            LOG("Not all assets have been processed!");
            cb(AL_ERR);
          }
      }
  });
  Sys::runOnWorkerThread([=]() mutable -> void {
    pak.commit();
    LOG("Start getting assets.");
    Network::ALL_DOWNLOADS.push_back(pak);
  });
}

void
copyAssets(Flow *flow, FlowCallback cb)
{
  cb(AL_COPYASSETS);
  std::string assetIndexSrc = flow->data[AL_FLOWVAR_ASSETINDEX];
  if(assetIndexSrc.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  std::list<Profile::Asset> assets = Profile::loadAssetIndex(assetIndexSrc);
  LOG("Mapping " << assets.size() << " legacy assets.");
  for(auto &a : assets)
    {
      auto src = getStoragePath("assets/objects/" + a.hash.substr(0, 2) + "/"
                                + a.hash);
      auto target = getInstallPath("assets/legacy/" + a.name);
      mkParentDirs(target);
      try
        {
          std::filesystem::copy_file(src, target);
        }
      catch(std::exception &ignored)
        {
        }
    }
  LOG("Mapped assets.");
  cb(AL_OK);
}

}
}