#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_NATIVESCHECK
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_NATIVESCHECK

#include "ach/core/op/Flow.hh"

namespace Alicorn
{

namespace Op
{

// Collect the native libraries to the profile root
void collectNatives(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_NATIVESCHECK */
