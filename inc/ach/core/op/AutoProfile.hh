#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_AUTOPROFILE
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_AUTOPROFILE

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Flow task to install specified modloader.
// Forge should not use this, use autoForge instead.
void autoProfile(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_AUTOPROFILE */
