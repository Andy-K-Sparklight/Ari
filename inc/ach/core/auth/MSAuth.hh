#ifndef INCLUDE_ALICORN_CH_ACH_CORE_AUTH_MSAUTH
#define INCLUDE_ALICORN_CH_ACH_CORE_AUTH_MSAUTH

#include "ach/core/profile/AccountProfile.hh"

#include <functional>
#include <string>

namespace Alicorn
{
namespace Auth
{

void msAuth(Profile::AccountProfile &acc);

void getMSCode(const std::string &part, std::function<void(std::string)> cb);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_AUTH_MSAUTH */
