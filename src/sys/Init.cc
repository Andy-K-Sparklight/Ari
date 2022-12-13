#include "ach/sys/Init.hh"

#include "ach/drivers/aria2/Aria2Driver.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Finder.hh"
#include "ach/sys/Storage.hh"
#include "ach/core/network/Download.hh"
#include "ach/core/profile/JVMProfile.hh"
#include "ach/core/profile/LaunchProfile.hh"
#include <log.hh>

namespace Alicorn
{
namespace Sys
{

void
initSys()
{
  LOG("Initializing system.");

  // Output build info

  // Services
  Sys::loadConfig();
  Sys::setupUVThread();
  Op::initBasePath();
  Sys::loadGlobalData(Profile::JVM_PROFILES, ACH_JVM_PROFILE);
  Sys::loadGlobalData(Profile::LAUNCH_PROFILES, ACH_LAUNCH_PROFILE);
  Network::setupDownloadsSync();
  Sys::runOnUVThread(
      []() -> void { AlicornDrivers::Aria2::ARIA2_DAEMON.run(); });
  LOG("Finished system initializing.")
}

void
downSys()
{
  LOG("Stopping system.");
  Network::stopDownloadsSync();
  Sys::saveGlobalData(Profile::LAUNCH_PROFILES, ACH_LAUNCH_PROFILE);
  Sys::saveGlobalData(Profile::JVM_PROFILES, ACH_JVM_PROFILE);
  Sys::runOnUVThread(
      []() -> void { AlicornDrivers::Aria2::ARIA2_DAEMON.stop(); });
  Sys::stopUVThread();
  Sys::saveConfig();
  LOG("System stopped.");
}

}
}