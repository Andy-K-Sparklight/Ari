#include "ach/sys/Init.hh"

#include "ach/drivers/aria2/Aria2Driver.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Finder.hh"
#include "ach/core/network/Download.hh"

namespace Alicorn
{
namespace Sys
{

void
initSys()
{
  Sys::setupUVThread();
  Op::initBasePath();
  Network::setupDownloadsSync();
  Sys::runOnUVThread(
      []() -> void { AlicornDrivers::Aria2::ARIA2_DAEMON.run(); });
}

void
downSys()
{
  Network::stopDownloadsSync();
  Sys::runOnUVThread(
      []() -> void { AlicornDrivers::Aria2::ARIA2_DAEMON.stop(); });
  Sys::stopUVThread();
}

}
}