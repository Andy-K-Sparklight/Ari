#include "ach/sys/Init.hh"

#include "ach/drivers/aria2/Aria2Driver.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Finder.hh"
#include "ach/sys/Storage.hh"
#include "ach/core/network/Download.hh"
#include "ach/core/profile/JVMProfile.hh"
#include <log.hh>

namespace Alicorn
{
namespace Sys
{

void
initSys()
{
  LOG("Initializing system.");
  Sys::loadConfig();
  Sys::setupUVThread();
  Op::initBasePath();
  Profile::loadJVMProfiles();
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
  Sys::runOnUVThread(
      []() -> void { AlicornDrivers::Aria2::ARIA2_DAEMON.stop(); });
  Profile::saveJVMProfiles();
  Sys::stopUVThread();
  Sys::saveConfig();
  LOG("System stopped.");
}

}
}