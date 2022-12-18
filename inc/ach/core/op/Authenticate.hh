#ifndef INCLUDE_ALICORN_CH_ACH_CORE_OP_AUTHENTICATE
#define INCLUDE_ALICORN_CH_ACH_CORE_OP_AUTHENTICATE

#include "ach/core/op/Flow.hh"

namespace Alicorn
{
namespace Op
{

// Flow task to authenticate account
void authAccount(Flow *flow, FlowCallback cb);

// Flow task to prefetch custom authlib server
void authlibPrefetch(Flow *flow, FlowCallback cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_OP_AUTHENTICATE */
