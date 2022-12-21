#include "ach/core/op/JVMCheck.hh"

#include "ach/core/profile/JVMProfile.hh"
#include "ach/core/platform/OSType.hh"
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
  Profile::JVMProfile *current = nullptr;
  bool exact = false;

  if(Platform::OS_TYPE == Platform::OS_DARWIN
     && Platform::OS_ARCH == Platform::AR_ARM)
    {
      if(profile.armSupport)
        {
          // Prefer ARM, but not absolutely
          for(auto &j : Profile::JVM_PROFILES)
            {
              if(j.specVersion == profile.javaVersion)
                {
                  current = &j;
                  if(j.isARM)
                    {
                      // Gotcha!
                      exact = true;
                      break;
                    }
                }
            }
          if(current == nullptr)
            {
              for(auto &j : Profile::JVM_PROFILES)
                {
                  if(j.specVersion > profile.javaVersion)
                    {
                      current = &j;
                      break; // A complete search has been done before
                    }
                }
            }
        }
      else
        {
          // We must use x64 variant
          for(auto &j : Profile::JVM_PROFILES)
            {
              if(!j.isARM)
                {
                  if(j.specVersion == profile.javaVersion)
                    {
                      // You, you...
                      current = &j;
                      exact = true;
                      break;
                    }
                  else if(j.specVersion > profile.javaVersion)
                    {
                      // Might still be available
                      current = &j;
                    }
                }
            }
        }
    }
  else
    {
      // Regular platform just do a normal search
      for(auto &j : Profile::JVM_PROFILES)
        {
          if(j.specVersion == profile.javaVersion)
            {
              // You, you...
              current = &j;
              exact = true;
              break;
            }
          else if(j.specVersion > profile.javaVersion)
            {
              // Might still be available
              current = &j;
            }
        }
    }

  if(current != nullptr)
    {
      flow->data[AL_FLOWVAR_JAVAMAIN] = current->bin;
      if(exact)
        {
          LOG("Found exact match JVM version " << current->specVersion
                                               << " at " << current->bin);
        }
      else
        {
          if(Platform::OS_TYPE == Platform::OS_DARWIN
             && Platform::OS_ARCH == Platform::AR_ARM
             && current->specVersion == profile.javaVersion)
            {
              LOG("Found transpiled JVM version " << current->specVersion
                                                  << " at " << current->bin);
            }
          else
            {
              LOG("Found possibly compatible JVM version "
                  << current->specVersion << " over requirement "
                  << profile.javaVersion << " at " << current->bin);
            }
        }
    }
  else
    {
      LOG("Could not find compatible JVM, using fallback.");
    }

  // Anyway, if failed we'll simply leave it empty and use env
  cb(AL_OK);
}
}
}