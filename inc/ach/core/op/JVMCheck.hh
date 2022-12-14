#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_JVMCHECK
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_JVMCHECK

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Select proper JVM based on profile
// If JVM has been explicitly specified this will be skipped
void selectJVM(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_JVMCHECK */
