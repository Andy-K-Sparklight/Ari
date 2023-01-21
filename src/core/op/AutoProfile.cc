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

// This will clean the target path.
// Make sure to do a flip before using this!
static void
mirrorDir(const std::string &name)
{
  auto base = Platform::getStoragePath(name);
  auto target = Platform::getInstallPath(name);
  if(std::filesystem::exists(target))
    {
      std::filesystem::remove_all(target);
    }
  Platform::mkParentDirs(target);
  std::filesystem::copy(base, target,
                        std::filesystem::copy_options::overwrite_existing
                            | std::filesystem::copy_options::recursive);
}
void
autoProfile(Flow *flow, FlowCallback cb)
{
  auto variant = flow->data[AL_FLOWVAR_LOADERTYPE];
  auto mcv = flow->data[AL_FLOWVAR_PROFILEID];
  auto ldv = flow->data[AL_FLOWVAR_LOADERVER];
  if(variant.size() == 0)
    {
      LOG("No loader specified, skipped loader installation.");
      cb(AL_OK);
      return;
    }
  cb(AL_INSLOADER);
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
  else
    {
      LOG("Unsupported loader " << variant);
      cb(AL_ERR);
    }
}
}
}