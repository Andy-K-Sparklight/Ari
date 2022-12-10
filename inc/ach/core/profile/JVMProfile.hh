#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_JVMPROFILE
#define INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_JVMPROFILE

#include <string>
#include <list>
#include <map>
#include <functional>
#include <vector>

namespace Alicorn
{
namespace Profile
{

class JVMProfile
{
public:
  std::string id;
  std::string bin;                       // Main Executable File
  unsigned int specVersion = 0;          // The major version
  bool is64 = false, isServer = false;   // x64 + Server VM is recommended
  std::list<std::string> versionStrings; // Usually 3 lines

  JVMProfile(const std::map<std::string, std::string> &slice);

  JVMProfile();

  std::map<std::string, std::string> toMap();
};

// Gets information automatically
// Uses sync method
bool appendJVM(const std::string &bin);

void loadJVMProfiles();

void saveJVMProfiles();

extern std::vector<JVMProfile> JVM_PROFILES;

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_JVMPROFILE */
