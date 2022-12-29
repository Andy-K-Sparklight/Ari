#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_BOOTSTRAP
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_BOOTSTRAP

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Configure launch based on specified profile
void configureLaunch(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_BOOTSTRAP */
