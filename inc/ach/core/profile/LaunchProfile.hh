#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_LAUNCHPROFILE
#define INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_LAUNCHPROFILE

#include <string>
#include <list>
#include <vector>
#include <map>

namespace Alicorn
{
namespace Profile
{

#define ACH_LAUNCH_PROFILE "LaunchProf.kvg"

class LaunchProfile
{

public:
  std::string id; // ID of this profile

  std::string displayName;     // The display name of this profile
  std::string icon;            // The icon data
  std::string jvm;             // ID of JVM profile
  std::list<std::string> mods; // ID of collected mods in a folder
  std::string runtime;         // ID of runtime
  std::string account;         // ID of account profile

  std::string baseProfile;   // The core to be launched
  std::string vmArgs;        // Extra VM args, split in spaces
  bool isDemo = false;       // Optn for demo
  int width = 0, height = 0; // Window size

  LaunchProfile(const std::map<std::string, std::string> &slice);

  LaunchProfile();

  std::map<std::string, std::string> toMap();
};

extern std::vector<LaunchProfile> LAUNCH_PROFILES;

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_LAUNCHPROFILE */
