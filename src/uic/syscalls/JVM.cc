#include "ach/uic/SysCall.hh"
#include "ach/uic/Protocol.hh"
#include "ach/core/op/Flow.hh"
#include "ach/extra/env/AutoJVM.hh"
#include "ach/core/profile/JVMProfile.hh"
#include "ach/uic/syscalls/JVM.hh"

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

#define ACH_SCANJAVA_VAR "%JRECheckResult"

void
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

void
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

}
}
}