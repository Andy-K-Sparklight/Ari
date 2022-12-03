#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_LIBRARIESINSTALL
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_LIBRARIESINSTALL

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Install specified libraries according to the profile
// Flow task
void installLibraries(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_LIBRARIESINSTALL */
