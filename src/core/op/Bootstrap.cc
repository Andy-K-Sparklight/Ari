#include "ach/core/op/Bootstrap.hh"

#include <log.hh>
#include "ach/core/profile/LaunchProfile.hh"
#include "ach/core/profile/AccountProfile.hh"
#include "ach/core/profile/JVMProfile.hh"

namespace Alicorn
{
namespace Op
{

void
configureLaunch(Flow *flow, FlowCallback cb)
{
  cb(AL_CFGLP);
  auto lpid = flow->data[AL_FLOWVAR_LCPROFILE];
  LOG("Configuring launch profile for " << lpid);
  Profile::LaunchProfile lp;
  for(auto &p : Profile::LAUNCH_PROFILES)
    {
      if(p.id == lpid)
        {
          lp = p;
          break;
        }
    }
  if(lp.id.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  flow->data[AL_FLOWVAR_PROFILEID] = lp.baseProfile;

  // We need to do the account index configure due to limitations
  for(size_t i = 0; i < Profile::ACCOUNT_PROFILES.size(); i++)
    {
      auto &a = Profile::ACCOUNT_PROFILES[i];
      if(a.id == lp.account)
        {
          flow->data[AL_FLOWVAR_ACCOUNTINDEX] = std::to_string(i);
        }
    }
  if(lp.width > 0 && lp.height > 0)
    {
      flow->data[AL_FLOWVAR_WIDTH] = std::to_string(lp.width);
      flow->data[AL_FLOWVAR_HEIGHT] = std::to_string(lp.height);
    }

  if(lp.jvm.size() > 0)
    {
      for(auto &j : Profile::JVM_PROFILES)
        {
          if(j.id == lp.jvm)
            {
              flow->data[AL_FLOWVAR_JAVAMAIN] = j.bin;
            }
        }
    }

  flow->data[AL_FLOWVAR_DEMO]
      = lp.isDemo ? std::string("1") : std::string("0");
  flow->data[AL_FLOWVAR_RUNTIME] = lp.runtime;
  flow->data[AL_FLOWVAR_EXVMARGS] = lp.vmArgs;
  LOG("Configured profile " << lp.id << " with game version "
                            << lp.baseProfile);
  cb(AL_OK);
}

}
}