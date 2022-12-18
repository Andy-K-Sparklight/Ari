#include "ach/core/op/Authenticate.hh"

#include "ach/core/profile/AccountProfile.hh"
#include "ach/core/auth/LocalAuth.hh"
#include "ach/core/auth/YggAuth.hh"
#include "ach/core/auth/MSAuth.hh"

#include <log.hh>
#include <httplib.h>
#include <lurl.hpp>

namespace Alicorn
{
namespace Op
{

static void
registerData(Profile::AccountProfile &acc,
             std::map<std::string, std::string> &data)
{
  LOG("Registering tokens with uuid " << acc.uuid);
  data[AL_FLOWVAR_AUTHNAME] = acc.userName;
  data[AL_FLOWVAR_AUTHTOKEN] = acc.mcToken;
  data[AL_FLOWVAR_AUTHUUID] = acc.uuid;
  data[AL_FLOWVAR_AUTHXUID] = acc.xuid;
}

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
  auto &acc = Profile::ACCOUNT_PROFILES[index];
  LOG("Authenticating account " << acc.id);
  if(acc.type == Profile::ACC_LOCAL)
    {
      LOG("Performing local authentication for " << acc.id);
      Auth::localAuth(acc);
      registerData(acc, flow->data);
      cb(AL_OK);
      return;
    }
  else if(acc.type == Profile::ACC_YGGDRASIL)
    {
      LOG("Performing Yggdrasil authentication for " << acc.id);
      if(acc.server.size() > 0)
        {
          flow->data[AL_FLOWVAR_AUTHSV] = acc.server;
        }
      if(Auth::yggAuth(acc, flow->data[AL_FLOWVAR_YGGPWD]))
        {
          LOG("Successfully authenticated with Yggdrasil.");
          registerData(acc, flow->data);
          cb(AL_OK);
          return;
        }
      else
        {
          LOG("Failed to authenticate with Yggdrasil.");
          cb(AL_ERR);
          return;
        }
    }
  else if(acc.type == Profile::ACC_MS)
    {
      LOG("Performing XBOX authentication for " << acc.id);
      Auth::msAuth(acc, [&, cb, flow](bool s) -> void {
        if(s)
          {
            LOG("Successfully authenticated with XBOX.")
            registerData(acc, flow->data);
            cb(AL_OK);
          }
        else
          {
            LOG("Failed to authenticate with XBOX.");
            cb(AL_ERR);
          }
      });
    }
}

void
authlibPrefetch(Flow *flow, FlowCallback cb)
{
  auto server = flow->data[AL_FLOWVAR_AUTHSV];
  if(server.size() > 0)
    {
      auto u = LUrlParser::ParseURL::parseURL(server);
      httplib::Client cli(u.connectionName_);
      auto res = cli.Get(u.pathName_);
      if(res != nullptr)
        {
          flow->data[AL_FLOWVAR_INJPREF] = res->body;
        }
    }
  cb(AL_OK); // This is optional, so we just callback anyway
}

}
}