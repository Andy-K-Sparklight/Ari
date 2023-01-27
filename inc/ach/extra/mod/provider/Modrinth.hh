#ifndef INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_PROVIDER_MODRINTH
#define INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_PROVIDER_MODRINTH

#include <string>
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

void dlModVersion(const std::string &v, std::function<void(bool)> cb);

void setupModrinthServer();

}
}
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_PROVIDER_MODRINTH */
