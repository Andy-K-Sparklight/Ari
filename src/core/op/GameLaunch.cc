#include "ach/core/op/GameLaunch.hh"

#include "ach/core/platform/OSType.hh"
#include "ach/util/Commons.hh"
#include "ach/core/op/Finder.hh"
#include "ach/core/profile/GameProfile.hh"
#include "ach/core/runtime/GameInstance.hh"
#include "ach/sys/Schedule.hh"
#include <regex>
#include <sstream>
#include <filesystem>
#include <map>
#include <iostream>

namespace Alicorn
{
namespace Op
{

typedef struct
{
  std::string xuid;
  std::string token;
  std::string uuid;
  std::string playerName;
  bool isDemo = false;
  bool hasCustomRes = false;
  std::string w, h;
  std::list<std::string> extraVMArgs;
  std::string clientID;
  std::string runtimeName;
} LaunchValues;

// The options are passed using JSON
static void
parseLaunchValues(const std::string &src, LaunchValues &ev)
{
  cJSON *obj = cJSON_Parse(src.c_str());

  cJSON *child;

  child = cJSON_GetObjectItem(obj, "xuid");
  if(cJSON_IsString(child))
    {
      ev.xuid = cJSON_GetStringValue(child);
    }

  child = cJSON_GetObjectItem(obj, "token");
  if(cJSON_IsString(child))
    {
      ev.token = cJSON_GetStringValue(child);
    }

  child = cJSON_GetObjectItem(obj, "uuid");
  if(cJSON_IsString(child))
    {
      ev.uuid = cJSON_GetStringValue(child);
    }

  child = cJSON_GetObjectItem(obj, "playerName");
  if(cJSON_IsString(child))
    {
      ev.playerName = cJSON_GetStringValue(child);
    }
  child = cJSON_GetObjectItem(obj, "w");
  if(cJSON_IsString(child))
    {
      ev.w = cJSON_GetStringValue(child);
    }
  child = cJSON_GetObjectItem(obj, "h");
  if(cJSON_IsString(child))
    {
      ev.h = cJSON_GetStringValue(child);
    }
  child = cJSON_GetObjectItem(obj, "clientID");
  if(cJSON_IsString(child))
    {
      ev.clientID = cJSON_GetStringValue(child);
    }
  child = cJSON_GetObjectItem(obj, "runtimeName");
  if(cJSON_IsString(child))
    {
      ev.runtimeName = cJSON_GetStringValue(child);
    }

  child = cJSON_GetObjectItem(obj, "isDemo");
  if(cJSON_IsBool(child))
    {
      ev.isDemo = cJSON_IsTrue(child);
    }

  child = cJSON_GetObjectItem(obj, "hasCustomRes");
  if(cJSON_IsBool(child))
    {
      ev.hasCustomRes = cJSON_IsTrue(child);
    }

  child = cJSON_GetObjectItem(obj, "extraVMArgs");
  if(cJSON_IsArray(child))
    {
      int l = cJSON_GetArraySize(child);
      for(int i = 0; i < l; i++)
        {
          cJSON *cur = cJSON_GetArrayItem(child, i);
          if(cJSON_IsString(cur))
            {
              ev.extraVMArgs.push_back(cJSON_GetStringValue(cur));
            }
        }
    }

  cJSON_Delete(obj);
}

static std::pair<bool, std::string>
getLibraryPath(const std::string &name)
{
  using namespace std::filesystem;
  path outPath;
  bool isNative;
  // Assemble name
  std::vector<std::string> nameParts = Commons::splitStr(name, ":");
  std::string libName;

  if(nameParts.size() == 4)
    {
      // Has natives, target is a folder
      isNative = true;
      libName = nameParts[1] + '-' + nameParts[2] + '-' + nameParts[3];
    }
  else
    {
      // No natives, target is a jar
      isNative = false;
      libName = nameParts[1] + '-' + nameParts[2] + ".jar";
    }

  // Assemble pkg path
  std::vector<std::string> pathParts = Commons::splitStr(nameParts[0], ".");
  outPath = getStoragePath("libraries");
  for(auto &p : pathParts)
    {
      outPath /= p;
    }

  // Pkg/Name/Ver
  outPath = outPath / nameParts[1] / nameParts[2] / libName;
  return { isNative, outPath.string() };
}

static bool
parseRule(const std::list<Profile::Rule> &rules, const LaunchValues &ext)
{
  using namespace Profile;
  if(rules.size() == 0)
    {
      return true;
    }
  bool fi = false;
  for(auto &r : rules)
    {
      switch(r.criteria)
        {
        case CR_ALWAYS:
          fi = true;
          break;
        case CR_FE_DEMO:
          if(ext.isDemo)
            {
              fi = r.action;
            }
          break;
        case CR_FE_CRES:
          if(ext.hasCustomRes)
            {
              fi = r.action;
            }
          break;
        case CR_OS_ARCH:
          if(Platform::OS_ARCH == Platform::AR_X86)
            {
              fi = r.action;
            }
          break;
        case CR_OS_NAME:
          if(r.condition == "linux" && Platform::OS_TYPE == Platform::OS_UNIX)
            {
              fi = r.action;
            }
          else if(r.condition == "osx"
                  && Platform::OS_TYPE == Platform::OS_DARWIN)
            {
              fi = r.action;
            }
          else if(r.condition == "windows"
                  && Platform::OS_TYPE == Platform::OS_MSDOS)
            {
              fi = r.action;
            }
          break;
        case CR_OS_VERSION:
          // I'm not DOS.
          break;
        }
    }
  return fi;
}

// Generate both natives and common libs
static std::pair<std::string, std::string>
genClassPath(const Profile::VersionProfile &prof, const LaunchValues &ext)
{
  using namespace std::filesystem;
  // Path split
  char split = ':';
  if(Platform::OS_TYPE == Platform::OS_MSDOS)
    {
      split = ';';
    }

  // Assign libs
  std::stringstream commonLibs, nativeLibs;
  for(auto &l : prof.libraries)
    {
      if(parseRule(l.rules, ext))
        {
          auto s = getLibraryPath(l.name);
          if(s.first)
            {
              nativeLibs << s.second << split;
              commonLibs << s.second + ".jarn"
                         << split; // Also copy one for some libs
            }
          else
            {
              commonLibs << s.second << split;
            }
        }
    }
  commonLibs << Op::getStoragePath(path("versions") / prof.id
                                   / (prof.id + ".jar"));
  std::string classPathCommon = commonLibs.str();
  std::string classPathNative = nativeLibs.str();

  if(classPathNative.ends_with(split))
    {
      classPathNative.pop_back();
    }
  return { classPathCommon, classPathNative };
}

static std::string
applyVars(const std::string &src,
          const std::map<std::string, std::string> &vars)
{
  std::string s = src;
  for(auto &kv : vars)
    {
      s = std::regex_replace(s, std::regex("\\$\\{" + kv.first + "\\}"),
                             kv.second);
    }
  return s;
}

// TODO: adjust to accept local
static std::list<std::string>
genArgs(const Profile::VersionProfile &prof, const LaunchValues &ev)
{
  std::list<std::string> finalArgs;
  std::map<std::string, std::string> varMap;

  // Vars assemble
  varMap["auth_player_name"] = ev.playerName;
  varMap["version_name"] = prof.id;
  varMap["game_directory"] = Op::getRuntimePath(ev.runtimeName);
  varMap["assets_root"] = Op::getStoragePath("assets");
  varMap["assets_index_name"]
      = prof.assetIndexArtifact.path; // This is actually id
  varMap["auth_uuid"] = ev.uuid;
  varMap["auth_access_token"] = ev.token;
  varMap["clientid"] = ev.clientID;
  varMap["auth_xuid"] = ev.xuid;
  varMap["user_type"] = "mojang";                         // Constant
  varMap["version_type"] = "Alicorn The Corrupted Heart"; // This is I!
  varMap["resolution_width"] = ev.w;
  varMap["resolution_height"] = ev.h;
  varMap["launcher_name"] = "Alicorn The Corrupted Heart";
  varMap["launcher_version"] = "Lost";
  auto classPaths = genClassPath(prof, ev);
  varMap["classpath"] = classPaths.first;
  varMap["natives_directory"] = classPaths.second;
  varMap["user_properties"] = "[]"; // Nothing, for 1.7
  varMap["game_assets"]
      = Op::getStoragePath(std::filesystem::path("assets") / "legacy");

  // Args processing
  // VM Args

  // Trivial flags
  finalArgs.push_back("-XX:+IgnoreUnrecognizedVMOptions");
  finalArgs.push_back("-XX:+UnlockExperimentalVMOptions");
  finalArgs.push_back("-Dfile.encoding=UTF-8");
  finalArgs.push_back("-showversion");

  // Profile JVM args
  for(auto &va : prof.jvmArgs)
    {
      if(parseRule(va.rules, ev))
        {
          for(auto &v : va.values)
            {
              finalArgs.push_back(applyVars(v, varMap));
            }
        }
    }

  // Externals
  for(auto &e : ev.extraVMArgs)
    {
      finalArgs.push_back(applyVars(e, varMap)); // Just in case
    }

  // In this world, the truly terrifying ones...
  // ...Are always and only the living beings, never the dead undead.
  finalArgs.push_back("-Dlog4j2.formatMsgNoLookups=true");
  finalArgs.push_back("-Dcom.sun.jndi.rmi.object.trustURLCodebase=false");
  finalArgs.push_back(
      "-Dcom.sun.jndi.cosnaming.object.trustURLCodebase=false");

  // Main class
  finalArgs.push_back(prof.mainClass);

  // Game Args
  for(auto &ga : prof.gameArgs)
    {
      if(parseRule(ga.rules, ev))
        {
          for(auto &v : ga.values)
            {
              finalArgs.push_back(applyVars(v, varMap));
            }
        }
    }

  return finalArgs;
}

void
launchGame(Flow *flow, FlowCallback cb)
{
  cb(AL_GENARGS);
  auto optnSrc = flow->data[AL_FLOWVAR_LAUNCHVALS];
  if(optnSrc.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  LaunchValues optn;
  parseLaunchValues(optnSrc, optn);

  auto profileSrc = flow->data[AL_FLOWVAR_PROFILESRC];
  if(profileSrc.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  cJSON *profileObj = cJSON_Parse(profileSrc.c_str());
  if(!cJSON_IsObject(profileObj))
    {
      cJSON_Delete(profileObj);
      cb(AL_ERR);
      return;
    }
  Profile::VersionProfile profile(profileObj);
  cJSON_Delete(profileObj);

  auto args = genArgs(profile, optn);
  Sys::runOnUVThread([=]() -> void {
    Runtime::GameInstance *game = new Runtime::GameInstance();
    cb(AL_SPAWNPROC);
    game->args = args; // Will be processed by launch module
    game->bin = flow->data[AL_FLOWVAR_JAVAMAIN];
    if(game->bin.size() == 0)
      {
        game->bin = "java"; // Use system
      }
    game->cwd = getRuntimePath(optn.runtimeName);
    try
      {

        std::filesystem::create_directories(game->cwd);
      }
    catch(std::exception &e)
      {
        game->cwd = "";
      }

    if(game->run())
      {
        Runtime::GAME_INSTANCES[game->proc.pid] = game;
        cb(AL_OK);
      }
    else
      {
        delete game;
        cb(AL_ERR);
      }
  });
}

}
}