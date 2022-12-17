#include "ach/core/profile/AccountProfile.hh"

namespace Alicorn
{
namespace Profile
{

AccountProfile::AccountProfile(const std::map<std::string, std::string> &slice)
{
  std::map<std::string, std::string> dat = slice;
  id = dat["id"];
  email = dat["email"];
  userName = dat["type"];
  uuid = dat["uuid"];
  mcToken = dat["mcToken"];
  refreshToken = dat["refreshToken"];
  xuid = dat["xuid"];
  type = (AccountType)std::stoi(dat["type"]);
}

AccountProfile::AccountProfile() {}

std::map<std::string, std::string>
AccountProfile::toMap()
{
  std::map<std::string, std::string> dat;
  dat["id"] = id;
  dat["email"] = email;
  dat["type"] = userName;
  dat["uuid"] = uuid;
  dat["mcToken"] = mcToken;
  dat["refreshToken"] = refreshToken;
  dat["xuid"] = xuid;
  dat["type"] = std::to_string((int)type);
  return dat;
}

std::vector<AccountProfile> ACCOUNT_PROFILES;

}
}