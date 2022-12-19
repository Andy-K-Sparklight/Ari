#ifndef INCLUDE_ALICORN_CH_ACH_UTIL_PROC
#define INCLUDE_ALICORN_CH_ACH_UTIL_PROC

#include <string>
#include <functional>
#include <list>

namespace Alicorn
{
namespace Commons
{

void runCommand(const std::string &bin, const std::list<std::string> &args,
                std::function<void(std::string, int)> cb, int pipe = 2);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UTIL_PROC */
