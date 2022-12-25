#ifndef INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_MODBURIN
#define INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_MODBURIN

#include <string>
#include <set>
#include <map>

namespace Alicorn
{
namespace Extra
{
namespace Mod
{

// Burin Structure
// .alicorn-ch/burin/
//   - metas
//     - AAAAAA
//   - versions
//     - BBBBBB
//   - files
//     - BBBBBB
//       - (Original Name)
//   - dynamics
//     - NNNNNN (ID of the launch profile)
//       - (Copied Files)

class ModMeta
{
public:
  std::string bid, displayName, desc;
  std::string provider, pid;      // ID on the platform, might be empty
  std::string slug;               // Aka. ModID, identify a mod
  std::set<std::string> versions; // Version IDs
  std::string icon;               // Base64 encoded icon data

  ModMeta(std::map<std::string, std::string> &slice);
  ModMeta();
  std::map<std::string, std::string> toMap();
};

class ModVersion
{
public:
  std::string bid, displayName;
  std::string pid;
  std::string slug;                            // Inherited From Meta
  std::set<std::string> urls;                  // All files, usually one
  std::set<std::string> gameVersions, loaders; // Supported

  ModVersion(std::map<std::string, std::string> &slice);
  ModVersion();
  std::map<std::string, std::string> toMap();
};

bool collectVersions(const std::set<std::string> vers, std::string lpid);
}
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_EXTRA_MOD_MODBURIN */
