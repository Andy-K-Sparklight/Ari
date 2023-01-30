#include "ach/core/auth/MSAuth.hh"

#include <httplib.h>
#include "ach/util/Proc.hh"
#include "ach/util/Commons.hh"
#include "ach/core/platform/Tools.hh"
#include <filesystem>
#include <cJSON.h>
#include <log.hh>

namespace Alicorn
{
namespace Auth
{

Profile::AccountProfile
mkMsAccount()
{
  Profile::AccountProfile p;
  p.type = Profile::ACC_MS;
  p.id = Commons::genUUID();
  return p;
}

static bool
completeAuth(Profile::AccountProfile &acc, const std::string &msToken)
{
  // Assume we've already got a valid MS token
  // XBL
  LOG("Authenticating with XBL.");
  httplib::Client xblCli("https://user.auth.xboxlive.com");
  httplib::Headers xblHeaders = { { "Content-Type", "application/json" },
                                  { "Accept", "application/json" } };

  std::string xblBody
      = "{\"Properties\":{\"AuthMethod\":\"RPS\",\"SiteName\":\"user.auth."
        "xboxlive.com\",\"RpsTicket\":\""
        + msToken
        + "\"},\"RelyingParty\":"
          "\"http://auth.xboxlive.com\",\"TokenType\":\"JWT\"}";
  xblCli.set_follow_location(true);
  auto xblRes = xblCli.Post("/user/authenticate", xblHeaders, xblBody,
                            "application/json");
  if(xblRes == nullptr || xblRes->status != 200)
    {
      return false;
    }
  cJSON *xblResContent = cJSON_Parse(xblRes->body.c_str());
  if(!cJSON_IsObject(xblResContent))
    {
      cJSON_Delete(xblResContent);
      return false;
    }
  cJSON *xblTokenObject = cJSON_GetObjectItem(xblResContent, "Token");
  if(!cJSON_IsString(xblTokenObject))
    {
      cJSON_Delete(xblResContent);
      return false;
    }
  std::string xblToken = cJSON_GetStringValue(xblTokenObject);
  cJSON *xblUhsObject = cJSON_GetObjectItem(
      cJSON_GetArrayItem(
          cJSON_GetObjectItem(
              cJSON_GetObjectItem(xblResContent, "DisplayClaims"), "xui"),
          0),
      "uhs");
  if(!cJSON_IsString(xblUhsObject))
    {
      cJSON_Delete(xblResContent);
      return false;
    }
  std::string xblUhs = cJSON_GetStringValue(xblUhsObject);
  cJSON_Delete(xblResContent);

  // XBOX For XUID
  LOG("Authenticating with XBOX.");
  httplib::Client xboxCli("https://xsts.auth.xboxlive.com");
  httplib::Headers xboxHeaders = xblHeaders; // Reuse
  std::string xboxBody
      = "{\"Properties\":{\"SandboxId\":\"RETAIL\",\"UserTokens\":[\""
        + xblToken
        + "\"]},\"RelyingParty\":\"http://xboxlive.com"
          "\",\"TokenType\":\"JWT\"}";
  xboxCli.set_follow_location(true);
  auto xboxRes = xboxCli.Post("/xsts/authorize", xboxHeaders, xboxBody,
                              "application/json");
  if(xboxRes == nullptr || xboxRes->status != 200)
    {
      return false;
    }
  cJSON *xboxResContent = cJSON_Parse(xboxRes->body.c_str());
  if(!cJSON_IsObject(xboxResContent))
    {
      cJSON_Delete(xboxResContent);
      return false;
    }
  // We only collect xuid
  cJSON *xuidObject = cJSON_GetObjectItem(
      cJSON_GetArrayItem(
          cJSON_GetObjectItem(
              cJSON_GetObjectItem(xboxResContent, "DisplayClaims"), "xui"),
          0),
      "xid");
  if(!cJSON_IsString(xuidObject))
    {
      cJSON_Delete(xboxResContent);
      return false;
    }
  acc.xuid = cJSON_GetStringValue(xuidObject);
  cJSON_Delete(xboxResContent);

  // XSTS For Token
  LOG("Authenticating with XSTS.");
  httplib::Client xstsCli("https://xsts.auth.xboxlive.com");
  httplib::Headers xstsHeaders = xboxHeaders; // Reuse
  std::string xstsBody
      = "{\"Properties\":{\"SandboxId\":\"RETAIL\",\"UserTokens\":[\""
        + xblToken
        + "\"]},\"RelyingParty\":\"rp://api.minecraftservices.com/"
          "\",\"TokenType\":\"JWT\"}";
  xstsCli.set_follow_location(true);
  auto xstsRes = xstsCli.Post("/xsts/authorize", xstsHeaders, xstsBody,
                              "application/json");
  if(xstsRes == nullptr || xstsRes->status != 200)
    {
      return false;
    }
  cJSON *xstsResContent = cJSON_Parse(xstsRes->body.c_str());
  if(!cJSON_IsObject(xstsResContent))
    {
      cJSON_Delete(xstsResContent);
      return false;
    }
  // We only collect xuid
  cJSON *xstsTokenObject = cJSON_GetObjectItem(xstsResContent, "Token");
  if(!cJSON_IsString(xstsTokenObject))
    {
      cJSON_Delete(xstsResContent);
      return false;
    }
  std::string xstsToken = cJSON_GetStringValue(xstsTokenObject);
  cJSON_Delete(xstsResContent);

  // Auth with Minecraft For Token
  LOG("Authenticating with Mojang.");
  httplib::Client mojangCli("https://api.minecraftservices.com");
  httplib::Headers mojangHeaders = xstsHeaders; // Reuse
  std::string mojangBody
      = "{\"identityToken\":\"XBL3.0 x=" + xblUhs + ";" + xstsToken + "\"}";
  mojangCli.set_follow_location(true);
  auto mojangRes
      = mojangCli.Post("/authentication/login_with_xbox", mojangHeaders,
                       mojangBody, "application/json");
  if(mojangRes == nullptr || mojangRes->status != 200)
    {
      return false;
    }

  cJSON *mojangResContent = cJSON_Parse(mojangRes->body.c_str());
  if(!cJSON_IsObject(mojangResContent))
    {
      cJSON_Delete(mojangResContent);
      return false;
    }
  cJSON *mcTokenObject = cJSON_GetObjectItem(mojangResContent, "access_token");
  if(!cJSON_IsString(mcTokenObject))
    {
      cJSON_Delete(mojangResContent);
      return false;
    }
  acc.mcToken = cJSON_GetStringValue(mcTokenObject);
  cJSON_Delete(mojangResContent);

  // Auth With Mojang
  LOG("Querying UUID and name.");
  httplib::Client mcCli("https://api.minecraftservices.com");
  httplib::Headers mcHeaders
      = { { "Content-Type", "application/json" },
          { "Authorization", "Bearer " + acc.mcToken } };
  mcCli.set_follow_location(true);
  auto mcRes = mcCli.Get("/minecraft/profile", mcHeaders);
  if(mcRes == nullptr || mcRes->status != 200)
    {
      std::cout << mcRes->status << std::endl;
      return false;
    }
  cJSON *mcResContent = cJSON_Parse(mcRes->body.c_str());
  if(!cJSON_IsObject(mcResContent))
    {
      cJSON_Delete(mcResContent);
      return false;
    }
  cJSON *uuidObj = cJSON_GetObjectItem(mcResContent, "id");
  cJSON *nameObj = cJSON_GetObjectItem(mcResContent, "name");
  if(!cJSON_IsString(uuidObj) || !cJSON_IsString(nameObj))
    {
      cJSON_Delete(mcResContent);
      return false;
    }
  acc.uuid = cJSON_GetStringValue(uuidObj);
  acc.userName = cJSON_GetStringValue(nameObj);
  cJSON_Delete(mcResContent);
  LOG("Authentication completed. Hello, " << acc.userName << "!");
  return true;
}

void
msAuth(Profile::AccountProfile &acc, std::function<void(bool)> cb)
{
  acc.type = Profile::ACC_MS;
  if(acc.refreshToken.size() > 0)
    {
      // Let's try refreshing
      LOG("Try refreshing token.")
      httplib::Client refreshCli("https://login.live.com");
      refreshCli.set_follow_location(true);
      auto refreshRes = refreshCli.Post(
          "/oauth20_token.srf",
          "client_id=00000000402b5328&refresh_token=" + acc.refreshToken
              + "&grant_type=refresh_token&redirect_uri=https%3A%2F%2Flogin."
                "live.com%2Foauth20_desktop.srf&scope=service%3A%3Auser.auth."
                "xboxlive.com%3A%3AMBI_SSL",
          "application/x-www-form-urlencoded");
      if(refreshRes != nullptr && refreshRes->status == 200)
        {
          LOG("Refresh OK, continuing authentication.");
          cJSON *refreshResContent = cJSON_Parse(refreshRes->body.c_str());
          cJSON *acTokenObj
              = cJSON_GetObjectItem(refreshResContent, "access_token");
          cJSON *rfTokenObj
              = cJSON_GetObjectItem(refreshResContent, "refresh_token");
          if(cJSON_IsString(acTokenObj) && cJSON_IsString(rfTokenObj))
            {
              acc.refreshToken = cJSON_GetStringValue(rfTokenObj);
              std::string token = cJSON_GetStringValue(acTokenObj);
              cb(completeAuth(acc, token));
              cJSON_Delete(refreshResContent);
              return;
            }
          cJSON_Delete(refreshResContent);
        }
    }

  getMSCode(acc.id, [=, &acc](std::string code) -> void {
    if(code.size() == 0)
      {
        LOG("Unexpected empty code, what have you done?");
        cb(false);
        return;
      }
    // First, finish the 2nd part of MSA
    LOG("Authenticating with MSA.")
    httplib::Client tokenCli("https://login.live.com");
    tokenCli.set_follow_location(true);
    auto tokenRes = tokenCli.Post(
        "/oauth20_token.srf",
        "client_id=00000000402b5328&code=" + code
            + "&grant_type=authorization_code&redirect_uri=https%3A%2F%"
              "2Flogin.live.com%2Foauth20_desktop.srf&scope=service%3A%3Auser."
              "auth.xboxlive.com%3A%3AMBI_SSL",
        "application/x-www-form-urlencoded");
    if(tokenRes != nullptr && tokenRes->status == 200)
      {
        LOG("Token retreived, continuing authentication.");
        cJSON *tokenResContent = cJSON_Parse(tokenRes->body.c_str());
        cJSON *acTokenObj
            = cJSON_GetObjectItem(tokenResContent, "access_token");
        cJSON *rfTokenObj
            = cJSON_GetObjectItem(tokenResContent, "refresh_token");
        if(cJSON_IsString(acTokenObj) && cJSON_IsString(rfTokenObj))
          {
            acc.refreshToken = cJSON_GetStringValue(rfTokenObj);
            std::string token = cJSON_GetStringValue(acTokenObj);
            cb(completeAuth(acc, token));
            cJSON_Delete(tokenResContent);
            return;
          }
        cJSON_Delete(tokenResContent);
      }
    cb(false);
  });
}

void
getMSCode(const std::string &part, std::function<void(std::string)> cb)
{
  try
    {
      auto loginExec = Commons::normalizePath(
          std::filesystem::temp_directory_path()
          / ("ach-ms-login/MSLogin!" + part + ".prog"));
      if(std::filesystem::exists(loginExec))
        {
          std::filesystem::remove(loginExec);
        }
      Platform::mkParentDirs(loginExec);
      std::filesystem::copy_file(Commons::argv0, loginExec);

      Commons::runCommand(
          loginExec, { "mslogin" }, [=](std::string c, int) -> void { cb(c); },
          1);
    }
  catch(std::exception &ignored)
    {
      cb("");
    }
}

}
}