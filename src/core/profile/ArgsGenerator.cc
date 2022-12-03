#include "ach/core/profile/ArgsGenerator.hh"

#include "ach/core/platform/OSType.hh"
#include <regex>
#include <sstream>
#include <map>

namespace Alicorn
{
namespace Profile
{

static bool
parseRule(const std::list<Rule> &rules, const ExternalValues &ext)
{
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
genClassPath(const VersionProfile &prof, const Box::GameBox &box,
             const ExternalValues &ext)
{
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
          auto s = box.getLibraryPath(l.name);
          if(s.first)
            {
              nativeLibs << s.second << split;
            }
          else
            {
              commonLibs << s.second << split;
            }
        }
    }
  commonLibs << box.getVersionClientPath(prof.id); // The client path
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
std::list<std::string>
genArgs(const VersionProfile &prof, const Box::GameBox &box,
        const ExternalValues &ev)
{
  std::list<std::string> finalArgs;
  std::map<std::string, std::string> varMap;

  // Vars assemble
  varMap["auth_player_name"] = ev.playerName;
  varMap["version_name"] = prof.id;
  varMap["game_directory"] = box.resolvePath("."); // Absolute path
  varMap["assets_root"] = box.getAssetsRootPath();
  varMap["assets_index_name"]
      = prof.assetIndexArtifact.path; // This is actually id
  varMap["auth_uuid"] = ev.uuid;
  varMap["auth_access_token"] = ev.token;
  varMap["clientid"] = ev.clientID;
  varMap["auth_xuid"] = ev.xuid;
  varMap["user_type"] = "mojang"; // Fixed value
  varMap["version_type"] = "Alicorn The Corrupted Heart";
  varMap["resolution_width"] = ev.w;
  varMap["resolution_height"] = ev.h;
  varMap["launcher_name"] = "Alicorn The Corrupted Heart";
  varMap["launcher_version"] = "Lost";
  auto classPaths = genClassPath(prof, box, ev);
  varMap["classpath"] = classPaths.first;
  varMap["natives_directory"] = classPaths.second;
  varMap["user_properties"] = "[]"; // Nothing
  varMap["game_assets"] = box.getLegacyAssetsRootPath();

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
  // ...Are always only the living beings, never the dead undead.
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
}
}