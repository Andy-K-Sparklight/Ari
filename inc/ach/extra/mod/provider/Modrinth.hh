#ifndef INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_PROVIDER_MODRINTH
#define INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_PROVIDER_MODRINTH

#include <string>
#include <set>
#include <functional>

namespace Alicorn
{
namespace Extra
{
namespace Mod
{
namespace Modrinth
{

bool getModMeta(const std::string &pid);

bool syncModVersions(const std::string &pid);

void dlModVersion(const std::set<std::string> &v,
                  std::function<void(bool)> cb);

void setupModrinthServer();

void showModrinthWindow(std::function<void()> cb);

}
}
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_PROVIDER_MODRINTH */
