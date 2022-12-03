#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_PROFILEINSTALL
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_PROFILEINSTALL

#include <string>
#include <functional>
#include "ach/core/op/Flow.hh"

namespace Alicorn
{

namespace Op
{

#define ACH_MOJANG_VERSION_MANIFEST_HOST "https://piston-meta.mojang.com"
#define ACH_MOJANG_VERSION_MANIFEST_PATH "/mc/game/version_manifest.json"

// Fetch the version manifest. Blocking.
std::string fetchVersionManifest();

// Install the profile to install path.
// Flow task
void installProfile(Flow *flow, FlowCallback sender);

// Install the client using previous url
// Flow task
void installClient(Flow *flow, FlowCallback sender);

}

}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_PROFILEINSTALL */
