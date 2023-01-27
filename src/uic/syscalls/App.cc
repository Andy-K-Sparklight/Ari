#include "ach/uic/syscalls/App.hh"

#define WEBVIEW_HEADER
#include <wv.hh>
#include "ach/sys/Schedule.hh"
#include "ach/uic/Protocol.hh"
#include "ach/sys/Init.hh"

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void
implExit(ACH_SC_ARGS)
{
  cb();
  prog.eip = prog.bin.size(); // Make sure to stop the program
  Sys::runOnMainThread(
      []() -> void {
        Sys::saveData();
        Sys::downSys();
        // Since webview_terminate might not reserve time for us
        auto mw = getMainWindow();
        webview_destroy(mw);
        webview_terminate(mw);
      },
      getMainWindow());
}

}
}
}