#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_FINDER
#define INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_FINDER

#include <filesystem>
#include "ach/core/profile/GameProfile.hh"

#define ACH_PT_VERSIONS "versions"
#define ACH_PT_LIBS "libs"

namespace Alicorn
{
namespace Platform
{

void initBasePath();

std::string getStoragePath();
std::string getStoragePath(const std::string &rel);
std::string getStoragePath(const std::filesystem::path &rel);
std::string getStoragePath(const char *rel);

std::string getTempPath(const std::string &rel);

std::string getInstallPath();
std::string getInstallPath(const std::string &rel);
std::string getInstallPath(const std::filesystem::path &rel);
std::string getInstallPath(const char *rel);

std::string getRuntimePath(const std::string &name);
std::string getJVMPath(const std::string &name);
std::string getJVMHomePath();

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_FINDER */
