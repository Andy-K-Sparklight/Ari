#include "ach/sys/Init.hh"

#include "ach/drivers/aria2/Aria2Driver.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/platform/Finder.hh"
#include "ach/sys/Storage.hh"
#include "ach/core/network/Download.hh"
#include "ach/core/profile/JVMProfile.hh"
#include "ach/core/profile/LaunchProfile.hh"
#include "ach/core/profile/AccountProfile.hh"
#include "ach/uic/UserData.hh"
#include "ach/uic/SysCall.hh"
#include "ach/extra/mod/provider/Modrinth.hh"
#include <log.hh>

namespace Alicorn
{
namespace Sys
{

static bool UP = false;

void
initSys()
{
  if(!UP)
    {
      LOG("Initializing system.");

      // Services
      srand(time(NULL));
      UIC::loadUserData();
      UIC::bindAllSysCalls();
      Sys::setupUVThread();
      Platform::initBasePath();
      Sys::loadGlobalData(Profile::JVM_PROFILES, ACH_JVM_PROFILE);
      Sys::loadGlobalData(Profile::LAUNCH_PROFILES, ACH_LAUNCH_PROFILE);
      Sys::loadGlobalData(Profile::ACCOUNT_PROFILES, ACH_ACCOUNT_PROFILE);
      Network::setupDownloadsSync();
      // During startup we can run on main thread
      AlicornDrivers::Aria2::ARIA2_DAEMON
          = new AlicornDrivers::Aria2::Aria2Daemon;
      AlicornDrivers::Aria2::ARIA2_DAEMON->run();
      Sys::runOnWorkerThread(
          []() -> void { Extra::Mod::Modrinth::setupModrinthServer(); });
      LOG("Finished system initializing.")
      UP = true;
    }
}

void
saveData()
{
  if(UP)
    {
      LOG("Saving data!");
      Sys::saveGlobalData(Profile::ACCOUNT_PROFILES, ACH_ACCOUNT_PROFILE);
      Sys::saveGlobalData(Profile::LAUNCH_PROFILES, ACH_LAUNCH_PROFILE);
      Sys::saveGlobalData(Profile::JVM_PROFILES, ACH_JVM_PROFILE);
      UIC::saveUserData();
    }
}

void
downSys()
{
  if(UP)
    {
      LOG("Stopping system.");
      Network::stopDownloadsSync();
      AlicornDrivers::Aria2::ARIA2_DAEMON->stop();
      Sys::stopUVThread();
      LOG("System stopped.");
      UP = false;
    }
}

void
macOSSaveData()
{
#ifdef __APPLE__
  Sys::runOnWorkerThread([]() -> void { saveData(); });
#endif
}

}
}