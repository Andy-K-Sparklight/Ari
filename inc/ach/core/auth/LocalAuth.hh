#ifndef INCLUDE_ALICORN_CH_ACH_CORE_AUTH_LOCALAUTH
#define INCLUDE_ALICORN_CH_ACH_CORE_AUTH_LOCALAUTH

#include "ach/core/profile/AccountProfile.hh"

namespace Alicorn
{
namespace Auth
{

// Create a new local account
Profile::AccountProfile mkLocalAccount(const std::string &userName);

// Authenticate local account sync
void localAuth(Profile::AccountProfile &acc);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_AUTH_LOCALAUTH */
