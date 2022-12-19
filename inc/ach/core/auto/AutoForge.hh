#ifndef INCLUDE_ALICORN_CH_ACH_CORE_AUTO_AUTOFORGE
#define INCLUDE_ALICORN_CH_ACH_CORE_AUTO_AUTOFORGE

#include <list>
#include <string>
#include <functional>

namespace Alicorn
{
namespace Auto
{

// Use some magic to get forge versions
// Unlike others, this is the full url of installer
std::list<std::string> getForgeVersions(const std::string &mcVersion);

// Use the url to install Forge.
void autoForge(const std::string &url, std::function<void(bool)> cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_AUTO_AUTOFORGE */
