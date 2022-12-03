#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_NATIVESCHECK
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_NATIVESCHECK

#include "ach/core/op/Flow.hh"

namespace Alicorn
{

namespace Op
{

// Scan the library directory and extract necessary libs
// It does not rely on the profile!
// Flow task
void checkNatives(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_NATIVESCHECK */
