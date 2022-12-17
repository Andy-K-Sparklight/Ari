#include "ach/core/op/Authenticate.hh"

#include "ach/core/profile/AccountProfile.hh"
#include "ach/core/auth/LocalAuth.hh"

#include <log.hh>

namespace Alicorn
{
namespace Op
{

void
authAccount(Flow *flow, FlowCallback cb)
{
  cb(AL_AUTHENTICATE);
  auto selectedID
      = flow->data[AL_FLOWVAR_ACCOUNTINDEX]; // That's the corresponding index
                                             // in the vector
  if(selectedID.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  int index = std::stoi(selectedID);
  auto acc = Profile::ACCOUNT_PROFILES[index];
  LOG("Authenticating account " << acc.id);
  if(acc.type == Profile::ACC_LOCAL)
    {
      LOG("Performing local authentication for " << acc.id);
      Auth::localAuth(acc);
    }
  // TODO: Other account types

  // Assemble data
  LOG("Registering tokens with uuid " << acc.uuid);
  flow->data[AL_FLOWVAR_AUTHNAME] = acc.userName;
  flow->data[AL_FLOWVAR_AUTHTOKEN] = acc.mcToken;
  flow->data[AL_FLOWVAR_AUTHUUID] = acc.uuid;
  flow->data[AL_FLOWVAR_AUTHXUID] = acc.xuid;
  cb(AL_OK);
}

}
}