#include "ach/uic/SysCall.hh"

#define WEBVIEW_HEADER
#include <wv.hh>
#include "ach/uic/Protocol.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Flow.hh"
#include "ach/extra/env/AutoJVM.hh"
#include "ach/core/profile/JVMProfile.hh"
#include <log.hh>

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

#define ACH_SCANJAVA_VAR "%JRECheckResult"

static void
implScanJava(ACH_SC_ARGS)
{
  Op::Flow *flow = new Op::Flow;
  flow->data[AL_FLOWVAR_DLJVM] = "0";
  flow->onProgress = ACH_DEFAULT_PROGRESS;
  flow->addTask(Extra::Env::configureJVM);
  flow->run([&prog, cb, flow](bool) -> void {
    bool has8, has17;
    if(Profile::JVM_PROFILES.size() == 0)
      {
        prog.locals[ACH_SCANJAVA_VAR] = "No";
      }
    for(auto &p : Profile::JVM_PROFILES)
      {
        if(p.specVersion == 8)
          {
            has8 = true;
          }
        else if(p.specVersion >= 17)
          {
            has17 = true;
          }
      }
    if(has8 && has17)
      {
        prog.locals[ACH_SCANJAVA_VAR] = "OK";
      }
    else
      {
        prog.locals[ACH_SCANJAVA_VAR] = "Miss";
      }
    cb();
    delete flow;
  });
}

static void
implInstallJRE(ACH_SC_ARGS)
{
  Op::Flow *flow = new Op::Flow;
  flow->data[AL_FLOWVAR_DLJVM] = "1";
  flow->addTask(Extra::Env::configureJVM);
  flow->onProgress = ACH_DEFAULT_PROGRESS;
  flow->run([&prog, cb](bool suc) -> void {
    prog.carry = suc;
    cb();
  });
}

void
bindAllSysCalls()
{
  bindSysCall("Exit", implExit);
  bindSysCall("ScanJava", implScanJava);
  bindSysCall("InstallJRE", implInstallJRE);
}

}
}