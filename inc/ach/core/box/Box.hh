#ifndef INCLUDE_ALICORN_CH_ACH_CORE_BOX_BOX
#define INCLUDE_ALICORN_CH_ACH_CORE_BOX_BOX

#include <string>

namespace Alicorn
{
namespace Box
{

// Like container in Alicorn
class GameBox
{
public:
  std::string root;
  std::string id;
  bool locked = false; // If another task is performing, do not operate it

  // Base resolver
  std::string resolvePath(const std::string &relativePath) const;

  // Bindings
  std::string
  getVersionProfilePath(const std::string &vid) const; // <VERSION>.json
  std::string
  getVersionClientPath(const std::string &vid) const; // <VERSION>.jar
  std::string
  getVersionExtProfilePath(const std::string &vid) const; // <VERSION>.ext.json
  std::pair<bool, std::string> getLibraryPath(const std::string &name)
      const; // <PATH>/<NAME>-<VER>.jar, natives are supported
  std::string getNativeLibraryPath(const std::string &name) const;
  std::string getAssetsRootPath() const;
  std::string getLegacyAssetsRootPath() const; // assets/legacy

  // ACH specials
  std::string getInstallTempPath(const std::string &id) const;
};

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_BOX_BOX */
