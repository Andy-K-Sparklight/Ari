#include "ach/core/op/Finder.hh"

#include <functional>
#include <algorithm>
#include <iostream>
#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Op
{

#define ACH_ROOT_NAME ".alicorn-ch"
#define ACH_STORAGE_PATH "storage"
#define ACH_INSTALL_PATH "installing"
#define ACH_GAME_PATH "gamemd"

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

static std::hash<std::string> stringHasher;

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
getNameHash(const std::string &name)
{
  return "" + stringHasher(name);
}

std::string
getRuntimePath(const std::string &name)
{
  return Commons::normalizePath(ACH_BASE_PATH / ACH_GAME_PATH / name);
}

}
}