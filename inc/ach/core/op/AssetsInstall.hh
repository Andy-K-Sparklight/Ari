#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_ASSETSINSTALL
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_ASSETSINSTALL

#include "ach/core/op/Flow.hh"
#include "ach/core/op/Finder.hh"

namespace Alicorn
{
namespace Op
{

void installAssetIndex(Flow *flow, FlowCallback cb);

void installAssets(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_ASSETSINSTALL */
