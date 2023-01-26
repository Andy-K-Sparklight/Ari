#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_FLOW
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_FLOW

// Each represents a stage
// If the code is not 0, then the last stage reported is where it happened.
#define AL_ERR -1
#define AL_OK 0

// Definitions of stages and vars
#define AL_MANIFEST 0x01
#define AL_FINDPROF 0x02
#define AL_GETPROF 0x03
#define AL_GETCLIENT 0x04
#define AL_UNPACKNAT 0x06
#define AL_GETLIBS 0x07
#define AL_GETASSETINDEX 0x08
#define AL_GETASSETS 0x09
#define AL_GENARGS 0x0a
#define AL_SPAWNPROC 0x0b
#define AL_FLIPDIR 0x0c
#define AL_COLNAT 0x0d
#define AL_SELECTJVM 0x0e
#define AL_RDPROFILE 0x0f
#define AL_EXTPROFILE 0x10
#define AL_LINKCLIENT 0x11
#define AL_AUTHENTICATE 0x12
#define AL_COPYASSETS 0x14
#define AL_LOADASSETIND 0x15
#define AL_DLJVM 0x16
#define AL_SCANJVM 0x17
#define AL_CFGLP 0x18
#define AL_INSLOADER 0x19

#define AL_FLOWVAR_PROFILEID "profileID"
#define AL_FLOWVAR_ASSETINDEX "assetIndex"
#define AL_FLOWVAR_JAVAMAIN "javaMain"
#define AL_FLOWVAR_INHCLIENTSRC "inhClientSrc"
#define AL_FLOWVAR_ACCOUNTINDEX "accountIndex"
#define AL_FLOWVAR_AUTHTOKEN "authToken"
#define AL_FLOWVAR_AUTHUUID "authUUID"
#define AL_FLOWVAR_AUTHNAME "authName"
#define AL_FLOWVAR_AUTHXUID "authXUID"
#define AL_FLOWVAR_RUNTIME "runtime"
#define AL_FLOWVAR_WIDTH "width"
#define AL_FLOWVAR_HEIGHT "height"
#define AL_FLOWVAR_DEMO "demo"
#define AL_FLOWVAR_EXVMARGS "exVMArgs"
#define AL_FLOWVAR_YGGPWD "yggPWD"
#define AL_FLOWVAR_AUTHSV "authSV"
#define AL_FLOWVAR_INJPREF "injPref"
#define AL_FLOWVAR_LOADERTYPE "loaderType"
#define AL_FLOWVAR_LOADERVER "loaderVer"
#define AL_FLOWVAR_DLJVM "dlJVM"
#define AL_FLOWVAR_LCPROFILE "lcProfile"
#define AL_FLOWVAR_GAMEPID "gamePID"

#include <map>
#include <string>
#include <list>
#include <functional>
#include "ach/core/network/Download.hh"

namespace Alicorn
{
namespace Op
{

class Flow;

typedef std::function<void(int)> FlowCallback;

typedef std::function<void(Flow *flow, FlowCallback cb)> FlowTask;

typedef std::function<void(double)> FlowProgress;
typedef std::function<void(int)> FlowStep;

// A flow is a tracker to complete the install progress
// It will collect files and track progress of each task
// This is a endpoint, there are no return value or callback.
// Instead, data will be sent directly to the frontend.
class Flow
{
protected:
  // Task list. Made protected to avoid unexpected change
  std::list<FlowTask> tasks;

public:
  unsigned int id; // Self ID
  std::map<std::string, std::string> data;
  unsigned int completedStage = 0, totalStage = 0; // Counter
  std::list<int> output;           // Outputs containing stage ID
  Profile::VersionProfile profile; // Extended var
  FlowProgress onProgress = nullptr;
  FlowStep onStep = nullptr;

  void run(std::function<void(bool)> cb = nullptr);

  void addTask(FlowTask t);
};

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_FLOW */
