#ifndef INCLUDE_ALICORN_CH_ACH_EXTRA_AUTOJVM
#define INCLUDE_ALICORN_CH_ACH_EXTRA_AUTOJVM

#include <list>
#include <string>
#include <functional>

namespace Alicorn
{
namespace Extra
{

// Scan for existing JVM installation.
void scanJVM(std::function<void(std::list<std::string>)> cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_EXTRA_AUTOJVM */
