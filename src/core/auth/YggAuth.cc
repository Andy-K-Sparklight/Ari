#include "ach/core/auth/YggAuth.hh"

#include "ach/util/Commons.hh"
#include <httplib.h>
#include <cJSON.h>
#include <log.hh>
#include <lurl.hpp>

namespace Alicorn
{
namespace Auth
{

Profile::AccountProfile
mkYggAccount(const std::string &email, const std::string &server)
{
  Profile::AccountProfile p;
  p.type = Profile::ACC_YGGDRASIL;
  p.email = email;
  p.id = Commons::genUUID();
  p.server = server;
  return p;
}

static bool
extractCredits(Profile::AccountProfile &acc, const std::string &response)
{
  LOG("Valid response received, parsing.");
  cJSON *resObj = cJSON_Parse(response.c_str());
  cJSON *tokenObj = cJSON_GetObjectItem(resObj, "accessToken");
  cJSON *selectedProfileObj = cJSON_GetObjectItem(resObj, "selectedProfile");
  if(!cJSON_IsObject(selectedProfileObj))
    {
      cJSON_Delete(resObj);
      return false;
    }
  cJSON *uuidObj = cJSON_GetObjectItem(selectedProfileObj, "id");
  cJSON *nameObj = cJSON_GetObjectItem(selectedProfileObj, "name");
  if(!cJSON_IsString(tokenObj) || !cJSON_IsString(uuidObj)
     || !cJSON_IsString(nameObj))
    {
      cJSON_Delete(resObj);
      return false;
    }

  acc.mcToken = cJSON_GetStringValue(tokenObj);
  acc.uuid = cJSON_GetStringValue(uuidObj);
  acc.userName = cJSON_GetStringValue(nameObj);
  acc.xuid = "0";
  LOG("Authentication completed. Hello, " << acc.userName << "!");
  return true;
}

bool
yggAuth(Profile::AccountProfile &acc, const std::string &pp)
{
  std::string server = acc.server;
  server += "/authserver"; // Suffix for authlib-injector
  LOG("Authenticating Yggdrasil account with server " << server);
  auto u = LUrlParser::ParseURL::parseURL(server);

  httplib::Client cli(u.connectionName_);
  if(acc.mcToken.size() > 0)
    {
      LOG("Trying refresh token.");
      // First try refresh
      auto res = cli.Post(u.pathName_ + "/refresh",
                          "{\"accessToken\": \"" + acc.mcToken
                              + "\",\"requestUser\":true}",
                          "application/json");
      if(res != nullptr && res->status == 200)
        {
          return extractCredits(acc, res->body);
        }
    }
  // Let's continue
  LOG("Authenticating using name and password.");
  auto res = cli.Post(
      u.pathName_ + "/authenticate",
      "{\"agent\": {\"name\":\"Minecraft\",\"version\":1},\"username\":\""
          + acc.email + "\",\"password\":\"" + pp + "\",\"requestUser\":true}",
      "application/json");
  if(res == nullptr || res->status != 200)
    {
      return false;
    }
  return extractCredits(acc, res->body);
}

}
}