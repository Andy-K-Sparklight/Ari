#include "ach/core/auto/AutoLoader.hh"

#include "ach/core/platform/Finder.hh"
#include "ach/core/platform/Tools.hh"
#include <httplib.h>
#include <cJSON.h>
#include <lurl.hpp>
#include <fstream>
#include <log.hh>

namespace Alicorn
{
namespace Auto
{

LoaderSuite FABRIC_SUITE
    = { .baseURL = "https://meta.fabricmc.net/v2", .name = "Fabric" },
    QUILT_SUITE
    = { .baseURL = "https://meta.quiltmc.org/v3", .name = "Quilt" };

std::list<std::string>
getLoaderVersions(const std::string &mcVersion, LoaderSuite detail)
{
  LOG("Fetching version manifest for " << detail.name << " with game "
                                       << mcVersion);
  std::list<std::string> out;
  auto rootURL = detail.baseURL + "/versions/loader/" + mcVersion;
  auto u = LUrlParser::ParseURL::parseURL(rootURL);
  httplib::Client cli(u.connectionName_);
  auto res = cli.Get(u.pathName_);
  if(res != nullptr || res->status == 200)
    {
      cJSON *manifest = cJSON_Parse(res->body.c_str());
      if(cJSON_IsArray(manifest))
        {
          int sz = cJSON_GetArraySize(manifest);
          for(int i = 0; i < sz; i++)
            {
              cJSON *loaderVer = cJSON_GetObjectItem(
                  cJSON_GetObjectItem(cJSON_GetArrayItem(manifest, i),
                                      "loader"),
                  "version");
              if(cJSON_IsString(loaderVer))
                {
                  out.push_back(cJSON_GetStringValue(loaderVer));
                }
            }
        }
      cJSON_Delete(manifest);
    }
  LOG("Fetched " << out.size() << " versions available.");
  return out;
}

bool
installLoader(const std::string &mcVersion, const std::string &ldVersion,
              LoaderSuite detail)
{
  LOG("Downloading profile for game " << mcVersion << " and " << detail.name
                                      << " " << ldVersion);
  auto rootURL = detail.baseURL + "/versions/loader/" + mcVersion + "/"
                 + ldVersion + "/profile/json";
  auto u = LUrlParser::ParseURL::parseURL(rootURL);
  httplib::Client cli(u.connectionName_);
  auto res = cli.Get(u.pathName_);
  if(res != nullptr && res->status == 200)
    {
      std::string name = mcVersion + "-" + detail.name + "-" + ldVersion;
      auto target = Platform::getInstallPath("versions/" + name + "/" + name
                                             + ".json");
      Platform::mkParentDirs(target);
      std::ofstream f(target);
      if(!f.fail())
        {
          cJSON *body = cJSON_Parse(res->body.c_str());
          cJSON *id = cJSON_GetObjectItem(body, "id");
          if(cJSON_IsString(id))
            {
              cJSON_SetValuestring(id, name.c_str());
            }
          std::string cont = cJSON_Print(body);
          cJSON_Delete(body);
          f << cont;
          LOG("Written profile for game " << mcVersion << " and "
                                          << detail.name << " " << ldVersion);
          return true;
        }
    }
  LOG("Could not install for game " << mcVersion << " and " << detail.name
                                    << " " << ldVersion);
  return false;
}

}
}