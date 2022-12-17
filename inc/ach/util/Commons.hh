#ifndef INCLUDE_ALICORN_CH_ACH_UTIL_COMMONS
#define INCLUDE_ALICORN_CH_ACH_UTIL_COMMONS

#include <vector>
#include <string>
#include <filesystem>

namespace Alicorn
{
namespace Commons
{

extern std::string argv0;

std::vector<std::string> splitStr(const std::string &str,
                                  const std::string &del);

std::string normalizePath(const std::filesystem::path &pt);

std::string getNameHash(const std::string &name);

// Generate random UUID
std::string genUUID();

// Generate UUID base on name
std::string genUUID(const std::string &name);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UTIL_COMMONS */
