#include "ach/core/auto/AutoForge.hh"

#include "ach/core/network/Download.hh"
#include "ach/core/platform/Finder.hh"
#include "ach/util/Commons.hh"
#include "ach/util/Proc.hh"
#include "ach/core/platform/Tools.hh"
#include "ach/core/platform/OSType.hh"
#include "ach/core/profile/GameProfile.hh"
#include "ach/core/profile/JVMProfile.hh"
#include <cJSON.h>
#include <httplib.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <log.hh>
#include <regex>
#include <list>
#include <set>
#include "ach/sys/Schedule.hh"

namespace Alicorn
{
namespace Auto
{

// "They don't understand. They don't stand the chance."
// "They're in the lion's zen and we're out for blood..."

std::list<std::string>
getForgeVersions(const std::string &mcVersion)
{
  std::list<std::string> urls;
  LOG("Fetching Forge versions for " << mcVersion);
  httplib::Client cli("https://files.minecraftforge.net");
  auto res = cli.Get("/net/minecraftforge/forge/index_" + mcVersion + ".html");
  if(res != nullptr && res->status == 200)
    {
      std::string html = res->body;
      // Use the sword, to split the fire.
      std::regex urlRegex(
          "<br><a "
          "href=\"(https://maven.minecraftforge.net/net/minecraftforge/forge/"
          ".*/.*\\.jar)\">\\(Direct Download\\)</a>");

      std::sregex_iterator iter(html.begin(), html.end(), urlRegex);
      std::sregex_iterator end;
      while(iter != end)
        {
          std::string u;
          if((*iter).size() > 1)
            {
              u = (*iter)[1];
              if(u.contains("installer"))
                {
                  urls.push_back(u);
                }
            }
          ++iter;
        };
    }
  LOG("Fetched " << urls.size() << " entries for Forge.");
  return urls;
}

// Run Forge installer targeting to a bare game folder
// The callback string is the original Forge name for caller to rename
static void
runForgeInstaller(const std::string &java, const std::string &bin,
                  std::function<void(std::string)> cb)
{
  LOG("Creating temporary launcher profiles.");
  // First, fake a launcher profile
  auto lppt = Platform::getInstallPath("launcher_profiles.json");

  std::ofstream lpf(lppt);
  if(!lpf.fail())
    {
      lpf << "{\"profiles\":{}}";
    }
  lpf.close();

  // And we collect the versions
  auto originVersionsL
      = Platform::scanDirectory(Platform::getInstallPath("versions"));
  std::set<std::string> originVersions;
  for(auto &v : originVersionsL)
    {
      originVersions.insert(v);
    }
  // Then let's run the launcher
  auto harmony = Commons::normalizePath("./HarmonyForgeInstallerCLI.jar");
  std::string split = Platform::OS_TYPE == Platform::OS_MSDOS ? ";" : ":";
  std::list<std::string> args;
  std::vector<std::string> args = { "-cp", harmony + split + bin,
                                    "rarityeg.alicorn.ForgeInstallerWrapper",
                                    Platform::getInstallPath() };
  LOG("Running Forge installer with helper " << harmony);
  Commons::runCommand(
      java, args,
      [=](std::string output, int code) -> void {
        LOG("Forge installer exitted with code " << code);
        // Let's clear the garbage
        try
          {
            std::filesystem::remove(lppt);
            LOG("Removed temporary launcher profiles.");
          }
        catch(std::exception &ignored)
          {
          }
        if(code != 0)
          {
            LOG("Failed to finish Forge installation.");
            cb(""); // Failed!
            return;
          }
        // Now we collect the versions again
        auto nowVersions
            = Platform::scanDirectory(Platform::getInstallPath("versions"));
        for(auto &v : nowVersions)
          {
            if(v.ends_with(".json"))
              {
                if(!originVersions.contains(v))
                  {
                    // Gotcha!
                    std::string name = std::filesystem::path(v)
                                           .replace_extension("")
                                           .filename()
                                           .string();
                    LOG("Found appended profile name " << name);
                    cb(name);
                    return;
                  }
              }
          }
        LOG("Could not find newly appended profile, cancelling.")
        cb(""); // Not found
      },
      2);
}

static std::string
getJavaForForge()
{
  std::string bin;
  if(Profile::JVM_PROFILES.size() > 0)
    {
      for(auto &j : Profile::JVM_PROFILES)
        {
          if(j.specVersion == 8)
            {
              bin = j.bin; // We prefer 8 first
            }
        }
      if(bin.size() == 0)
        {
          bin = Profile::JVM_PROFILES[0].bin; // Just the first one
        }
    }
  if(bin.size() == 0)
    {
      bin = "java";
    }
  return bin;
}

static void
renameForge(const std::string &originName)
{
  // Origin name is <mcv>-forge-<fgv>
  LOG("Pretty renaming Forge profile.");
  auto verSplit = Commons::splitStr(originName, "-");

  std::string nn;

  // Legacy Forge name
  if((verSplit.size() == 3 && verSplit[2] == verSplit[0])
     || verSplit.size() == 2)
    {
      auto &mid = verSplit[1];
      if(mid.starts_with("Forge"))
        {
          mid = mid.substr(5);
          nn = verSplit[0] + "+Forge-" + mid;
        }
    }
  if(nn.size() == 0)
    {
      if(verSplit.size() == 3)
        {
          // New Forge name
          nn = verSplit[0] + "+Forge-" + verSplit[2];
        }
      else
        {
          return; // Cannot touch
        }
    }

  auto profilePt = Platform::getInstallPath("versions/" + originName + "/"
                                            + originName + ".json");
  std::ifstream f(profilePt);
  std::stringstream ss;
  ss << f.rdbuf();
  f.close();
  cJSON *dat = cJSON_Parse(ss.str().c_str());
  if(cJSON_IsObject(dat))
    {
      cJSON *id = cJSON_GetObjectItem(dat, "id");
      cJSON_SetValuestring(id, nn.c_str());
    }
  auto nProfilePt
      = Platform::getInstallPath("versions/" + nn + "/" + nn + ".json");
  Platform::mkParentDirs(nProfilePt);
  std::ofstream nf(nProfilePt);
  nf << cJSON_Print(dat);
  cJSON_Delete(dat);
  std::filesystem::remove_all(
      Platform::getInstallPath("versions/" + originName));
}

static void
cpMappings(const std::string &mcp, const std::string &game)
{
  LOG("Dumping game mappings for " << game);
  auto originalPt = Platform::getStoragePath("versions/" + game + "/" + game
                                             + ".mappings");
  auto targetPt = Platform::getInstallPath(
      "libraries/net/minecraft/client/" + game + "-" + mcp + "/client-" + game
      + "-" + mcp + "-mappings.txt");

  if(!std::filesystem::exists(targetPt))
    {
      Platform::mkParentDirs(targetPt);
      std::filesystem::copy_file(originalPt, targetPt);
    }
}

// Assume a pack has been initialized
static void
finalizeForgeRun(Network::DownloadPack lpack, std::string installerPath,
                 std::function<void(bool)> cb)
{
  lpack.assignUpdateCallback([=](const Network::DownloadPack *pk) -> void {
    auto stat = pk->getStat();
    if(stat.failed + stat.completed == stat.total)
      {
        if(stat.failed == 0)
          {
            // Continue running installer
            runForgeInstaller(getJavaForForge(), installerPath,
                              [=](std::string name) -> void {
                                LOG("Forge installer run completed.");
                                std::filesystem::remove_all(installerPath);
                                if(name.size() > 0)
                                  {
                                    renameForge(name);
                                    cb(true); // Done!
                                  }
                                else
                                  {
                                    cb(false);
                                  }
                              });
          }
        else
          {
            cb(false);
          }
      }
  });
  Sys::runOnWorkerThread([=]() mutable -> void {
    LOG("Crunching libraries for Forge.");
    lpack.commit();
    Network::ALL_DOWNLOADS.push_back(lpack);
  });
}

void
takeOverMappings(cJSON *install)
{
  LOG("Taking over mappings download.");
  cJSON *processors = cJSON_GetObjectItem(install, "processors");
  if(cJSON_IsArray(processors))
    {
      int sz = cJSON_GetArraySize(processors);
      for(int i = 0; i < sz; i++)
        {
          cJSON *args
              = cJSON_GetObjectItem(cJSON_GetArrayItem(processors, i), "args");
          if(cJSON_IsArray(args))
            {
              int asz = cJSON_GetArraySize(args);
              for(int j = 0; j < asz; j++)
                {
                  cJSON *c = cJSON_GetArrayItem(args, j);
                  if(cJSON_IsString(c))
                    {
                      std::string a = cJSON_GetStringValue(c);
                      if(a == "DOWNLOAD_MOJMAPS")
                        {
                          cJSON_DeleteItemFromArray(processors, i);
                          LOG("Removed useless op at " << i);
                          i--;
                          break;
                        }
                    }
                }
            }
        }
    }
}

void
autoForge(const std::string &url, std::function<void(bool)> cb)
{
  // First let's download it
  LOG("Automating Forge installation for " << url);
  Network::DownloadMeta forgeInstallerMeta;
  forgeInstallerMeta.baseURL = url;
  std::string installerName = "fgi-" + Commons::getNameHash(url) + ".jar";
  std::string installerPath = Platform::getTempPath(installerName);
  LOG(installerPath);
  forgeInstallerMeta.path = installerPath;
  Network::DownloadPack pk;
  pk.addTask(forgeInstallerMeta);
  pk.assignUpdateCallback([=](const Network::DownloadPack *pk) -> void {
    auto stat = pk->getStat();
    if(stat.completed + stat.failed == stat.total)
      {
        if(stat.failed == 0)
          {
            // OK, let's continue
            // Unpack the installer to somewhere
            LOG("Unpacking Forge installer.");
            std::string fgWorkFolder = Platform::getTempPath("ForgeWork");
            Platform::unzipFile(installerPath, fgWorkFolder);

            // Let's grab the profile
            auto installJSON
                = std::filesystem::path(fgWorkFolder) / "install_profile.json";
            std::stringstream installJSONStr;
            std::ifstream installJSONF(installJSON);
            if(installJSONF.fail())
              {
                cb(false);
                return;
              }
            installJSONStr << installJSONF.rdbuf();
            installJSONF.close();
            std::string installJSONData = installJSONStr.str();
            cJSON *install = cJSON_Parse(installJSONData.c_str());
            if(!cJSON_IsObject(install))
              {
                cJSON_Delete(install);
                cb(false);
                return;
              }

            // Detect variants
            if(cJSON_HasObjectItem(install, "data"))
              {
                // New
                LOG("New Forge install profile detected.");

                // First let's copy the mappings
                cJSON *mcpVerObj = cJSON_GetObjectItem(
                    cJSON_GetObjectItem(cJSON_GetObjectItem(install, "data"),
                                        "MCP_VERSION"),
                    "client");
                cJSON *mcVerObj = cJSON_GetObjectItem(install, "minecraft");
                if(cJSON_IsString(mcpVerObj) && cJSON_IsString(mcVerObj))
                  {
                    std::string mcpVer = cJSON_GetStringValue(mcpVerObj);
                    mcpVer.pop_back();
                    mcpVer = mcpVer.substr(1);

                    std::string gameVer = cJSON_GetStringValue(mcVerObj);
                    cpMappings(mcpVer, gameVer);
                  }

                // Let's also grab the version profile
                auto versionPt
                    = std::filesystem::path(fgWorkFolder) / "version.json";
                std::ifstream versionFile(versionPt.string());
                std::stringstream versionDat;
                versionDat << versionFile.rdbuf();
                versionFile.close();
                cJSON *version = cJSON_Parse(versionDat.str().c_str());

                // Now let's collect the libraries
                // This is quite incomplete, but we only want the libraries
                Profile::VersionProfile pf(install);
                Profile::VersionProfile pv(version);
                Network::DownloadPack lpack;
                for(auto &l : pf.libraries)
                  {
                    if(l.artifact.url.size() > 0)
                      {
                        auto mt = Network::DownloadMeta::mkFromLibrary(
                            l, Platform::getInstallPath("libraries"));
                        lpack.addTask(mt);
                      }
                  }
                for(auto &l : pv.libraries)
                  {
                    if(l.artifact.url.size() > 0)
                      {
                        auto mt = Network::DownloadMeta::mkFromLibrary(
                            l, Platform::getInstallPath("libraries"));
                        lpack.addTask(mt);
                      }
                  }
                LOG("Patching version profile.");
                cJSON *comment = cJSON_GetObjectItem(version, "_comment_");
                if(cJSON_IsArray(comment))
                  {
                    auto ap1
                        = cJSON_CreateString("And please bear in mind...");
                    auto ap2 = cJSON_CreateString("A forging hammer can never "
                                                  "break a piece of fabric.");
                    cJSON_AddItemToArray(comment, ap1);
                    cJSON_AddItemToArray(comment, ap2);
                  }

                std::ofstream versionFo(versionPt);
                versionFo << cJSON_Print(version);
                versionFo.close();
                cJSON_Delete(version);

                // Let the libraries fly for a while
                // Now repack the installer, first take over mappings
                takeOverMappings(install);
                std::string insStr = cJSON_Print(install);
                cJSON_Delete(install);
                std::ofstream outFile(installJSON);
                if(outFile.fail())
                  {
                    cb(false);
                    return;
                  }
                outFile << insStr;
                outFile.close();

                // Dispose sig
                std::filesystem::remove(installerPath); // Cleanup

                finalizeForgeRun(lpack, fgWorkFolder, cb);
                return;
              }
            else if(cJSON_HasObjectItem(install, "install"))
              {
                // Legacy
                LOG("Legacy Forge install profile detected.");

                // For this one, just fill the libraries
                cJSON *libraries = cJSON_GetObjectItem(
                    cJSON_GetObjectItem(install, "versionInfo"), "libraries");

                Network::DownloadPack lpack;

                if(cJSON_IsArray(libraries))
                  {
                    int sz = cJSON_GetArraySize(libraries);

                    // Collect libraries
                    for(int i = 0; i < sz; i++)
                      {
                        cJSON *lib = cJSON_GetArrayItem(libraries, i);
                        cJSON *clientReq
                            = cJSON_GetObjectItem(lib, "clientreq");

                        // Check clientreq
                        if(cJSON_IsBool(clientReq) && cJSON_IsTrue(clientReq))
                          {
                            // We just borrow the parser
                            Profile::Library clib(lib);
                            if(clib.artifact.url.size() > 0)
                              {
                                auto mt = Network::DownloadMeta::mkFromLibrary(
                                    clib,
                                    Platform::getInstallPath("libraries"));
                                lpack.addTask(mt);
                              };
                          }
                      }
                  }
                cJSON_Delete(install); // We no longer need it
                std::filesystem::remove_all(fgWorkFolder);
                finalizeForgeRun(lpack, installerPath, cb);
                return;
              }
            else
              {
                LOG("Unsupported Forge install profile detected, cannot "
                    "install.")
                cJSON_Delete(install);
                cb(false);
                return;
              }
          }
        else
          {
            // Network not good
            cb(false);
            return;
          }
      }
  });
  Sys::runOnWorkerThread([=]() mutable -> void {
    LOG("Start downloading installer from " << url);
    pk.commit();
    Network::ALL_DOWNLOADS.push_back(pk);
  });
}
}
}