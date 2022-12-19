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
#include "log.hh"

namespace Alicorn
{
namespace Op
{

// Parse rules
static bool
parseRule(const std::list<Profile::Rule> &rules,
          std::map<std::string, std::string> flowData)
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
          if(flowData[AL_FLOWVAR_DEMO] != "0")
            {
              fi = r.action;
            }
          break;
        case CR_FE_CRES:
          if(flowData[AL_FLOWVAR_WIDTH].size() > 0
             && flowData[AL_FLOWVAR_HEIGHT].size() > 0)
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
static std::string
genClassPath(const Profile::VersionProfile &prof,
             std::map<std::string, std::string> flowData)
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
      if(parseRule(l.rules, flowData))
        {
          commonLibs << getStoragePath("libraries/" + l.artifact.path)
                     << split;
        }
    }
  commonLibs << getStoragePath("versions/" + prof.clientArtifact.path)
             << split;

  std::string classPathCommon = commonLibs.str();
  return classPathCommon;
}

static std::string
applyVars(const std::string &src,
          const std::map<std::string, std::string> &vars)
{
  std::string s = src;
  for(auto &kv : vars)
    {
      std::regex k("\\$\\{" + kv.first + "\\}");
      while(std::regex_search(s, k))
        {
          s = std::regex_replace(s, k, kv.second); // Replace all
        }
    }
  return s;
}

static std::list<std::string>
genArgs(const Profile::VersionProfile &prof,
        std::map<std::string, std::string> &flowData)
{
  LOG("Generating args for " << prof.id);
  std::list<std::string> finalArgs;
  std::map<std::string, std::string> varMap;

  // Vars assemble
  varMap["auth_player_name"] = flowData[AL_FLOWVAR_AUTHNAME];
  varMap["version_name"] = prof.id;
  varMap["game_directory"] = getRuntimePath(flowData[AL_FLOWVAR_RUNTIME]);
  varMap["assets_root"] = getStoragePath("assets");
  varMap["assets_index_name"]
      = prof.assetIndexArtifact.path; // This is actually id
  varMap["auth_uuid"] = flowData[AL_FLOWVAR_AUTHUUID];
  varMap["auth_access_token"] = flowData[AL_FLOWVAR_AUTHTOKEN];
  varMap["auth_session"] = flowData[AL_FLOWVAR_AUTHTOKEN];
  varMap["clientid"] = "0"; // Not sure, reserved
  varMap["auth_xuid"] = flowData[AL_FLOWVAR_AUTHXUID];
  varMap["user_type"] = "mojang";
  std::string vType;
  if(prof.type == Profile::RT_RELEASE)
    {
      vType = "release";
    }
  else if(prof.type == Profile::RT_SNAPSHOT)
    {
      vType = "snapshot";
    }
  else if(prof.type == Profile::RT_ALPHA)
    {
      vType = "old_alpha";
    }
  else if(prof.type == Profile::RT_BETA)
    {
      vType = "old_beta";
    }
  varMap["version_type"] = vType; // Be honest
  varMap["resolution_width"] = flowData[AL_FLOWVAR_WIDTH];
  varMap["resolution_height"] = flowData[AL_FLOWVAR_HEIGHT];
  varMap["launcher_name"] = "Alicorn The Corrupted Heart";
  varMap["launcher_version"] = "Lost";
  auto classPaths = genClassPath(prof, flowData);
  LOG("Classpaths with length " << classPaths.size() << " generated.");
  varMap["classpath"] = classPaths;
  varMap["natives_directory"]
      = getStoragePath("versions/" + prof.id + "/.natives");
  varMap["user_properties"] = "[]";                        // Nothing, for 1.7
  varMap["game_assets"] = getStoragePath("assets/legacy"); // Pre 1.6

  // Below are Forge related vars
  varMap["library_directory"] = getStoragePath("libraries"); // Root path
  varMap["classpath_separator"]
      = Platform::OS_TYPE == Platform::OS_MSDOS ? ";" : ":";
  varMap["version_name"] = prof.id;

  // Args processing
  // VM Args

  // Trivial flags
  finalArgs.push_back("-XX:+IgnoreUnrecognizedVMOptions");
  finalArgs.push_back("-XX:+UnlockExperimentalVMOptions");
  finalArgs.push_back("-Dfile.encoding=UTF-8");
  finalArgs.push_back("-Duser.language=en");
  finalArgs.push_back("-showversion");

  // Authlib Injector
  auto &authServer = flowData[AL_FLOWVAR_AUTHSV];
  if(authServer.size() > 0)
    {
      finalArgs.push_back("-javaagent:"
                          + Commons::normalizePath("./authlib-injector.jar")
                          + "=" + authServer);
    }

  auto &prefetch = flowData[AL_FLOWVAR_INJPREF];
  if(prefetch.size() > 0)
    {
      finalArgs.push_back("-Dauthlibinjector.yggdrasil.prefetched="
                          + prefetch);
    }

  // Profile JVM args
  for(auto &va : prof.jvmArgs)
    {
      if(parseRule(va.rules, flowData))
        {
          for(auto &v : va.values)
            {
              finalArgs.push_back(applyVars(v, varMap));
            }
        }
    }

  // Process Extra Args
  auto vmx = flowData[AL_FLOWVAR_EXVMARGS];
  std::regex reg(" +");
  while(std::regex_search(vmx, reg))
    {
      vmx = std::regex_replace(vmx, reg, "\t\n");
    }

  // Externals
  for(auto &e : Commons::splitStr(vmx, "\t\n"))
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
      if(parseRule(ga.rules, flowData))
        {
          for(auto &v : ga.values)
            {
              finalArgs.push_back(applyVars(v, varMap));
            }
        }
    }
  LOG("Generated final args with " << finalArgs.size() << " members.");
  return finalArgs;
}

void
launchGame(Flow *flow, FlowCallback cb)
{
  cb(AL_GENARGS);
  Profile::VersionProfile profile = flow->profile;

  auto args = genArgs(profile, flow->data);
  Sys::runOnUVThread([=]() -> void {
    LOG("Creating game instance for " << profile.id);
    Runtime::GameInstance *game = new Runtime::GameInstance();
    cb(AL_SPAWNPROC);
    game->args = args; // Will be processed by launch module
    game->bin = flow->data[AL_FLOWVAR_JAVAMAIN];
    if(game->bin.size() == 0)
      {
        game->bin = "java"; // Use system
      }
    LOG("Using bin as " << game->bin);
    game->cwd = getRuntimePath(flow->data[AL_FLOWVAR_RUNTIME]);
    try
      {
        std::filesystem::create_directories(game->cwd);
      }
    catch(std::exception &e)
      {
        game->cwd = "";
      }
    if(game->cwd.size() > 0)
      {
        LOG("Set CWD to " << game->cwd);
      }

    if(game->run())
      {
        LOG("Successfully spawned game process with pid " << game->proc.pid);
        Runtime::GAME_INSTANCES[game->proc.pid] = game;
        cb(AL_OK);
      }
    else
      {
        LOG("Could not spawn game process for " << profile.id);
        delete game;
        cb(AL_ERR);
      }
  });
}

}
}