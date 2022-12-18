#ifndef INCLUDE_ALICORN_CH_ACH_CORE_AUTH_YGGAUTH
#define INCLUDE_ALICORN_CH_ACH_CORE_AUTH_YGGAUTH

#include "ach/core/profile/AccountProfile.hh"

namespace Alicorn
{
namespace Auth
{

Profile::AccountProfile mkYggAccount(const std::string &email,
                                     const std::string &server);

bool yggAuth(Profile::AccountProfile &acc, const std::string &pp);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_AUTH_YGGAUTH */
