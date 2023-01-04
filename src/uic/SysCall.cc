#include "ach/uic/SysCall.hh"

#define WEBVIEW_HEADER
#include <wv.hh>
#include "ach/uic/Protocol.hh"
#include "ach/sys/Schedule.hh"

namespace Alicorn
{
namespace UIC
{

#define ACH_SC_ARGS Program &prog, PCallback cb

static void
implExit(ACH_SC_ARGS)
{
  cb();
  prog.eip = prog.bin.size(); // Make sure to stop the program
  Sys::runOnMainThread(
      []() -> void {
        auto mw = getMainWindow();
        webview_destroy(mw);
        webview_terminate(mw);
      },
      getMainWindow());
}

void
bindAllSysCalls()
{
  bindSysCall("Exit", implExit);
}

}
}