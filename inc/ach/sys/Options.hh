#ifndef INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS
#define INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS

#include <map>
#include <string>

namespace Alicorn
{
namespace Sys
{

extern std::map<std::string, std::string> ACH_CONFIG;

void loadConfig();

std::string getValue(const std::string &k, const std::string &dv);

void setValue(const std::string &k, const std::string &v);

void saveConfig();
}
}
#endif /* INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS */
