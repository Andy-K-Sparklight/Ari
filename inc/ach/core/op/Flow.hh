#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_FLOW
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_FLOW

// Each represents a stage
// If the code is not 0, then the last stage reported is where it happened.
#define AL_ERR -1
#define AL_OK 0
#define AL_WARN 1

// Definitions of stages and vars
#define AL_MANIFEST 0x01
#define AL_FINDPROF 0x02
#define AL_GETPROF 0x03
#define AL_GETCLIENT 0x04
#define AL_SCANNAT 0x05
#define AL_UNPACKNAT 0x06
#define AL_GETLIBS 0x07
#define AL_GETASSETINDEX 0x08
#define AL_GETASSETS 0x09
#define AL_GENARGS 0x10
#define AL_SPAWNPROC 0x11
#define AL_FLIPDIR 0x12

#define AL_FLOWVAR_PROFILEID "profileID"
#define AL_FLOWVAR_PROFILESRC "profileSrc"
#define AL_FLOWVAR_ASSETINDEX "assetIndex"
#define AL_FLOWVAR_LAUNCHVALS "launchValues"
#define AL_FLOWVAR_JAVAMAIN "javaMain"

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
  std::list<int> output; // Outputs containing stage ID

  void run();

  void addTask(FlowTask t);

  // Status will be directly read by the creator of Flow and send
};

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_FLOW */
