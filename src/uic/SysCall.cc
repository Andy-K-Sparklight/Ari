#include "ach/uic/SysCall.hh"

#include "ach/uic/syscalls/App.hh"
#include "ach/uic/syscalls/JVM.hh"

namespace Alicorn
{
namespace UIC
{

void
bindAllSysCalls()
{
  bindSysCall("Exit", SysCall::implExit);
  bindSysCall("ScanJava", SysCall::implScanJava);
  bindSysCall("InstallJRE", SysCall::implInstallJRE);
}

}
}