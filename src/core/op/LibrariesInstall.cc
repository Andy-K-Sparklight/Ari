#include "ach/core/op/LibrariesInstall.hh"

#include "ach/core/profile/GameProfile.hh"
#include "ach/core/platform/OSType.hh"
#include "ach/core/network/Download.hh"
#include "ach/core/op/Finder.hh"
#include "ach/sys/Schedule.hh"
#include <iostream>

#include <cJSON.h>

namespace Alicorn
{
namespace Op
{

// A simpler library only rule parser
static bool
parseLibRule(const std::list<Profile::Rule> &rules)
{
  using namespace Profile;
  if(rules.size() == 0)
    {
      return true;
    }
  bool fi = false;
  for(auto &r : rules)
    {
      switch(r.criteria)
        {

        case CR_ALWAYS:
          fi = true;
          break;
          break;
        case CR_OS_ARCH:
          if(Platform::OS_ARCH == Platform::AR_X86)
            {
              fi = r.action;
            }
          break;
        case CR_OS_NAME:
          if(r.condition == "linux" && Platform::OS_TYPE == Platform::OS_UNIX)
            {
              fi = r.action;
            }
          else if(r.condition == "osx"
                  && Platform::OS_TYPE == Platform::OS_DARWIN)
            {
              fi = r.action;
            }
          else if(r.condition == "windows"
                  && Platform::OS_TYPE == Platform::OS_MSDOS)
            {
              fi = r.action;
            }
          break;
        case CR_OS_VERSION:
          // I'm not DOS.
        default:

          break;
        }
    }
  return fi;
}

void
installLibraries(Flow *flow, FlowCallback cb)
{
  cb(AL_GETLIBS);
  std::string profileSrc = flow->data[AL_FLOWVAR_PROFILESRC];
  if(profileSrc.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  cJSON *profileJ = cJSON_Parse(profileSrc.c_str());

  if(!cJSON_IsObject(profileJ))
    {
      cb(AL_ERR);
      return;
    }
  Profile::VersionProfile profile(profileJ);
  Network::DownloadPack librariesPack;
  for(auto &lib : profile.libraries)
    {
      if(parseLibRule(lib.rules))
        {
          auto meta = Network::DownloadMeta::mkFromArtifact(
              lib.artifact, getInstallPath("libraries"));
          librariesPack.addTask(meta);
        }
    }
  librariesPack.assignUpdateCallback(
      [=](const Network::DownloadPack *p) -> void {
        auto ps = p->getStat();
        if((ps.completed + ps.failed) == ps.total)
          {
            // All processed
            if(ps.failed == 0)
              {
                cb(AL_OK);
              }
            else
              {
                cb(AL_ERR);
              }
          }
      });
  Sys::runOnWorkerThread([=]() mutable -> void {
    librariesPack.commit();
    Network::ALL_DOWNLOADS.push_back(librariesPack);
  });
}

}
}