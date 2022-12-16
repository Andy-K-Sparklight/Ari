#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_PROFILEEXT
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_PROFILEEXT

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Load profile from local storage
void loadProfile(Flow *flow, FlowCallback cb);

// Copy the client to target place
void linkClient(Flow *flow, FlowCallback cb);
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_PROFILEEXT */
