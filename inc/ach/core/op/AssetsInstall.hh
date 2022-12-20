#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_ASSETSINSTALL
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_ASSETSINSTALL

#include "ach/core/op/Flow.hh"
#include "ach/core/op/Finder.hh"

#define ACH_MOJANG_RESOURCES_HOST "https://resources.download.minecraft.net"

namespace Alicorn
{
namespace Op
{

void installAssetIndex(Flow *flow, FlowCallback cb);

void loadAssetIndex(Flow *flow, FlowCallback cb);

void installAssets(Flow *flow, FlowCallback cb);

void copyAssets(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_ASSETSINSTALL */
