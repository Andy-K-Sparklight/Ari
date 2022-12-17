#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_ACCOUNTPROFILE
#define INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_ACCOUNTPROFILE

#include <string>
#include <map>
#include <vector>

#define ACH_ACCOUNT_PROFILE "AccProf.kvg"

namespace Alicorn
{
namespace Profile
{

enum AccountType
{
  ACC_MS = 0,
  ACC_YGGDRASIL = 1,
  ACC_LOCAL = 2,
};

class AccountProfile
{
public:
  std::string id;
  AccountType type;
  std::string email;
  std::string userName;
  std::string uuid, mcToken;
  std::string refreshToken, xuid; // For ms only

  AccountProfile(const std::map<std::string, std::string> &slice);

  AccountProfile();

  std::map<std::string, std::string> toMap();
};

extern std::vector<AccountProfile> ACCOUNT_PROFILES;

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_ACCOUNTPROFILE */
