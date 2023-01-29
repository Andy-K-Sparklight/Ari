#include "ach/core/platform/Finder.hh"

#include <functional>
#include <algorithm>
#include <iostream>
#include "ach/util/Commons.hh"
#include "ach/core/platform/OSType.hh"
#include <log.hh>

namespace Alicorn
{
namespace Platform
{

#define ACH_ROOT_NAME ".arimc"
#define ACH_STORAGE_PATH "storage"
#define ACH_INSTALL_PATH "installing"
#define ACH_GAME_PATH "gamemd"
#define ACH_JVM_PATH "jvm"

static std::filesystem::path ACH_BASE_PATH;

void
initBasePath()
{
  char *base = NULL;
  if((base = getenv("HOME")) != NULL)
    {
    }
  else if((base = getenv("APPDATA")) != NULL)
    {
    }
  else if((base = getenv("USERPROFILE")) != NULL)
    {
    }
  else
    {
      base = NULL; // Should never happen on a modern OS
    }
  ACH_BASE_PATH
      = std::filesystem::path(Commons::normalizePath(std::filesystem::absolute(
          std::filesystem::path(base == NULL ? "." : base) / ACH_ROOT_NAME)));
  LOG("Configured base path as " << ACH_BASE_PATH);
}

std::string
getStoragePath()
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_STORAGE_PATH);
}

std::string
getStoragePath(const std::string &rel)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_STORAGE_PATH / rel);
}

std::string
getStoragePath(const std::filesystem::path &rel)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_STORAGE_PATH / rel);
}

std::string
getStoragePath(const char *rel)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_STORAGE_PATH / rel);
}

std::string
getTempPath(const std::string &rel)
{
  return Commons::normalizePath(std::filesystem::temp_directory_path()
                                / ACH_ROOT_NAME / rel);
}

std::string
getInstallPath()
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_INSTALL_PATH);
}

std::string
getInstallPath(const std::string &rel)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_INSTALL_PATH / rel);
}

std::string
getInstallPath(const std::filesystem::path &rel)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_INSTALL_PATH / rel);
}

std::string
getInstallPath(const char *rel)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_INSTALL_PATH / rel);
}

std::string
getRuntimePath(const std::string &name)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_GAME_PATH / name);
}

std::string
getJVMPath(const std::string &name)
{
  auto root = ACH_BASE_PATH / ACH_JVM_PATH / ("jre" + name);
  if(Platform::OS_TYPE == Platform::OS_DARWIN)
    {
      root = root / "Contents/Home/bin/java";
    }
  else if(Platform::OS_TYPE == Platform::OS_MSDOS)
    {
      root = root / "bin/java.exe";
    }
  else if(Platform::OS_TYPE == Platform::OS_UNIX)
    {
      root = root / "bin/java";
    }

  return Commons::normalizePath(root);
}

std::string
getJVMHomePath()
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_JVM_PATH);
}

std::string
getBasePath()
{
  return Commons::normalizePath(ACH_BASE_PATH);
}

}
}