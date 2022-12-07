#ifndef INCLUDE_ALICORN_CH_ACH_UTIL_COMMONS
#define INCLUDE_ALICORN_CH_ACH_UTIL_COMMONS

#include <vector>
#include <string>
#include <filesystem>

namespace Alicorn
{
namespace Commons
{
std::vector<std::string> splitStr(const std::string &str,
                                  const std::string &del);

std::string normalizePath(const std::filesystem::path &pt);
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UTIL_COMMONS */
