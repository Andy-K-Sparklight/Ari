#include "ach/core/auth/LocalAuth.hh"

#include "ach/util/Commons.hh"
#include <uuid.h>
#include <log.hh>

namespace Alicorn
{
namespace Auth
{

Profile::AccountProfile
mkLocalAccount(const std::string &userName)
{
  Profile::AccountProfile p;
  p.id = Commons::genUUID();
  p.mcToken = Commons::getNameHash(Commons::genUUID()); // Let's play!
  p.uuid = Commons::genUUID("OfflinePlayer:" + userName);
  p.type = Profile::ACC_LOCAL;
  p.userName = userName;
  LOG("Created local account with name " << userName);
  return p;
}

void
localAuth(Profile::AccountProfile &acc)
{
  LOG("Authenticating local account " << acc.id);
  // Only token needs to be changed, others remain untouched
  acc.mcToken = Commons::getNameHash(Commons::genUUID()); // Refresh a new one
  acc.xuid = std::string("0");
}

}
}