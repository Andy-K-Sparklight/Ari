#include "ach/core/op/JVMCheck.hh"

#include "ach/core/profile/JVMProfile.hh"
#include <log.hh>

namespace Alicorn
{
namespace Op
{
void
selectJVM(Flow *flow, FlowCallback cb)
{
  cb(AL_SELECTJVM);
  if(flow->data[AL_FLOWVAR_JAVAMAIN].size() > 0)
    {
      // Already defined
      cb(AL_OK);
      return;
    }

  Profile::VersionProfile profile = flow->profile;
  LOG("Selecting JVM for " << profile.id);
  for(auto &p : Profile::JVM_PROFILES)
    {
      if(p.specVersion == profile.javaVersion)
        {

          // That's it, just take it
          flow->data[AL_FLOWVAR_JAVAMAIN] = p.bin;
          LOG("Selected JVM " << p.bin << " with exact version "
                              << p.specVersion);
          break;
        }
    }
  if(flow->data[AL_FLOWVAR_JAVAMAIN].size() == 0)
    {
      for(auto &p : Profile::JVM_PROFILES)
        {
          if(p.specVersion > profile.javaVersion)
            {
              // Will the latest work? Maybe.
              flow->data[AL_FLOWVAR_JAVAMAIN] = p.bin;
              LOG("Assumed JVM "
                  << p.bin << " with spec version " << p.specVersion
                  << " might supply required version " << profile.javaVersion);
              break;
            }
        }
    }
  // Anyway, if failed we'll simply leave it empty and use env
  cb(AL_OK);
}
}
}