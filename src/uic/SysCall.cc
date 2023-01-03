#include "ach/uic/SysCall.hh"

#define WEBVIEW_HEADER
#include <wv.hh>
#include "ach/uic/Protocol.hh"

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
  webview_terminate(getMainWindow());
}

void
bindAllSysCalls()
{
  bindSysCall("Exit", implExit);
}

}
}