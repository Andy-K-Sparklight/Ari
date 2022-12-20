#include "ach/core/op/AutoProfile.hh"

#include "ach/core/auto/AutoLoader.hh"
#include "ach/core/auto/AutoForge.hh"
#include "ach/util/Commons.hh"
#include "ach/core/platform/Tools.hh"
#include "ach/core/platform/Finder.hh"

#include <log.hh>

namespace Alicorn
{
namespace Op
{

static void
mirrorDir(const std::string &name)
{
  auto base = Platform::getStoragePath(name);
  auto files = Platform::scanDirectory(base);
  for(auto &f : files)
    {
      auto relPt = std::filesystem::relative(std::filesystem::path(f), base);
      auto target
          = std::filesystem::path(Platform::getInstallPath(name)) / relPt;
      if(!std::filesystem::exists(target))
        {
          Platform::mkParentDirs(target);
          std::filesystem::copy(f, target);
        }
    }
}
void
autoProfile(Flow *flow, FlowCallback cb)
{
  auto variant = flow->data[AL_FLOWVAR_LOADERTYPE];
  auto mcv = flow->data[AL_FLOWVAR_PROFILEID];
  auto ldv = flow->data[AL_FLOWVAR_LOADERVER];
  if(variant == "Fabric")
    {
      LOG("AutoProfile is installing Fabric " << ldv);
      if(!Auto::installLoader(mcv, ldv, Auto::FABRIC_SUITE))
        {
          LOG("Could not install Fabric " << ldv);
          cb(AL_ERR);
          return;
        }
      LOG("Installed Fabric " << ldv);
      cb(AL_OK);
      return;
    }
  else if(variant == "Quilt")
    {
      LOG("AutoProfile is installing Quilt " << ldv);
      if(!Auto::installLoader(mcv, ldv, Auto::QUILT_SUITE))
        {
          LOG("Could not install Quilt " << ldv);
          cb(AL_ERR);
          return;
        }
      LOG("Installed Quilt " << ldv);
      cb(AL_OK);
      return;
    }
  else if(variant == "Forge")
    {
      LOG("AutoProfile is installing Forge " << ldv);
      mirrorDir("versions");
      mirrorDir("libraries"); // Should be enough
      Auto::autoForge(ldv, [=](bool s) -> void {
        if(s)
          {
            LOG("Installed Forge " << ldv);
            cb(AL_OK);
          }
        else
          {
            LOG("Could not install Forge " << ldv);
            cb(AL_ERR);
          }
      });
    }
}
}
}