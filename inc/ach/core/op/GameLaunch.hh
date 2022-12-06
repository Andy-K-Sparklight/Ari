#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_GAMELAUNCH
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_GAMELAUNCH

#include <string>
#include <list>
#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Launch the game
// Flow task
void launchGame(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_GAMELAUNCH */
