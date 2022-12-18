#ifndef INCLUDE_ALICORN_CH_ACH_CORE_AUTO_AUTOLOADER
#define INCLUDE_ALICORN_CH_ACH_CORE_AUTO_AUTOLOADER

#include <list>
#include <string>

namespace Alicorn
{
namespace Auto
{

typedef struct
{
  std::string baseURL;
  std::string name;
} LoaderSuite;

std::list<std::string> getLoaderVersions(const std::string &mcVersion,
                                         LoaderSuite detail);

bool installLoader(const std::string &mcVersion, const std::string &ldVersion,
                   LoaderSuite detail);

extern LoaderSuite FABRIC_SUITE, QUILT_SUITE;

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_AUTO_AUTOLOADER */
