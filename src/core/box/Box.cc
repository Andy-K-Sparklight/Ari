#include "ach/core/box/Box.hh"

#include <filesystem>
#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Box
{

using namespace std::filesystem;
std::string
GameBox::resolvePath(const std::string &rel) const
{
  return (absolute(path(root)) / rel).string();
}

std::string
GameBox::getVersionProfilePath(const std::string &vid) const
{
  return (absolute(path(root)) / "versions" / vid / (vid + ".json")).string();
}

std::string
GameBox::getVersionClientPath(const std::string &vid) const
{
  return (absolute(path(root)) / "versions" / vid / (vid + ".jar")).string();
}

std::string
GameBox::getVersionExtProfilePath(const std::string &vid) const
{
  return (absolute(path(root)) / "versions" / vid / (vid + ".ext.json"))
      .string();
}

std::pair<bool, std::string>
GameBox::getLibraryPath(const std::string &name) const
{
  path outPath;
  bool isNative;
  // Assemble name
  std::vector<std::string> nameParts = Commons::splitStr(name, ":");
  std::string libName;

  if(nameParts.size() == 4)
    {
      // Has natives, target is a folder
      isNative = true;
      libName = nameParts[1] + '-' + nameParts[2] + '-' + nameParts[3];
    }
  else
    {
      // No natives, target is a jar
      isNative = false;
      libName = nameParts[1] + '-' + nameParts[2] + ".jar";
    }

  // Assemble pkg path
  std::vector<std::string> pathParts = Commons::splitStr(nameParts[0], ".");
  outPath = absolute(path(root)) / "libraries";
  for(auto &p : pathParts)
    {
      outPath /= p;
    }

  // Pkg/Name/Ver
  outPath = outPath / nameParts[1] / nameParts[2] / libName;
  return { isNative, outPath.string() };
}

std::string
GameBox::getAssetsRootPath() const
{
  return (absolute(path(root)) / "assets").string();
}

std::string
GameBox::getLegacyAssetsRootPath() const
{
  return (absolute(path(root)) / "assets" / "legacy").string();
}

std::string
GameBox::getInstallTempPath(const std::string &id) const
{
  return (absolute(path(root)) / "ach" / "install" / id).string();
}

}
}