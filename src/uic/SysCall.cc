#include "ach/uic/SysCall.hh"

#include "ach/uic/syscalls/App.hh"
#include "ach/uic/syscalls/JVM.hh"
#include "ach/uic/syscalls/Account.hh"
#include "ach/uic/syscalls/Game.hh"

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
  bindSysCall("AddLocalAccount", SysCall::implAddLocalAccount);
  bindSysCall("AddMSAccount", SysCall::implAddMSAccount);
  bindSysCall("ValidateYggServer", SysCall::implValidateYggServer);
  bindSysCall("AddYGAccount", SysCall::implAddYGAccount);
  bindSysCall("GetVersions", SysCall::implGetVersions);
  bindSysCall("GetLoaderList", SysCall::implGetLoaderList);
  bindSysCall("InstallProfile", SysCall::implInstallProfile);
  bindSysCall("GetAllAccounts", SysCall::implGetAllAccounts);
  bindSysCall("LaunchGame", SysCall::implLaunchGame);
  bindSysCall("MonitorGame", SysCall::implMonitorGame);
  bindSysCall("GetLaunchProfiles", SysCall::implGetLaunchProfiles);
  bindSysCall("OpenFolder", SysCall::implOpenFolder);
}

}
}