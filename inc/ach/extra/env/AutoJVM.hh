#ifndef INCLUDE_ALICORN_CH_ACH_EXTRA_ENV_AUTOJVM
#define INCLUDE_ALICORN_CH_ACH_EXTRA_ENV_AUTOJVM

#include "ach/core/op/Flow.hh"
#include <list>
#include <string>
#include <functional>

namespace Alicorn
{
namespace Extra
{

namespace Env
{

// Scan for existing JVM installation.
void scanJVM(std::function<void(std::list<std::string>)> cb);

// Install corresponding JVMs.
void installJVM(std::function<void(bool)> cb);

// Flow task to configure JVM
void configureJVM(Op::Flow *flow, Op::FlowCallback cb);

}
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_EXTRA_ENV_AUTOJVM */
