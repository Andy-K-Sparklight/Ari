#include "ach/core/op/AssetsInstall.hh"

#include <httplib.h>
#include <cJSON.h>
#include "ach/core/profile/GameProfile.hh"
#include "ach/core/op/Tools.hh"
#include "ach/sys/Schedule.hh"
#include <lurl.hpp>
#include <list>
#include <string>
#include <iostream>

namespace Alicorn
{
namespace Op
{

void
installAssetIndex(Flow *flow, FlowCallback cb)
{
  cb(AL_GETASSETINDEX);
  std::string src = flow->data[AL_FLOWVAR_PROFILESRC];
  if(src.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  cJSON *profileObj = cJSON_Parse(src.c_str());
  if(!cJSON_IsObject(profileObj))
    {
      cJSON_Delete(profileObj);
      cb(AL_ERR);
      return;
    }
  Profile::VersionProfile profile(profileObj);
  cJSON_Delete(profileObj);
  auto u = LUrlParser::ParseURL::parseURL(profile.assetIndexArtifact.url);
  httplib::Client cli(u.connectionName_);
  auto res = cli.Get(u.pathName_);
  if(res->status != 200)
    {
      cb(AL_ERR);
      return;
    }
  std::string assetIndexSrc = res->body;
  if(assetIndexSrc.size() == 0)
    {
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
          cb(AL_ERR);
        }
      else
        {
          cb(AL_OK);
        }
    });
  });
}

}
}