#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_FLIPPER
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_FLIPPER

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Apply changes from install path to storage path
void flipInstall(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_FLIPPER */
