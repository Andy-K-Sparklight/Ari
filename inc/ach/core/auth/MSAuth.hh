#ifndef INCLUDE_ALICORN_CH_ACH_CORE_AUTH_MSAUTH
#define INCLUDE_ALICORN_CH_ACH_CORE_AUTH_MSAUTH

#include "ach/core/profile/AccountProfile.hh"

#include <functional>
#include <string>

namespace Alicorn
{
namespace Auth
{

Profile::AccountProfile mkMsAccount();

void msAuth(Profile::AccountProfile &acc, std::function<void(bool)> cb);

void getMSCode(const std::string &part, std::function<void(std::string)> cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_AUTH_MSAUTH */
