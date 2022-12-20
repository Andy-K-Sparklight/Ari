#include "ach/core/profile/JVMProfile.hh"

#include "ach/sys/Storage.hh"
#include "ach/util/Commons.hh"
#include "ach/util/Proc.hh"
#include "ach/sys/Schedule.hh"
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <regex>
#include <uuid.h>
#include <log.hh>

namespace Alicorn
{
namespace Profile
{

JVMProfile::JVMProfile(const std::map<std::string, std::string> &slice)
{
  std::map<std::string, std::string> dat = slice;
  id = dat["id"];
  bin = dat["bin"];
  specVersion = std::stoi(dat["specVersion"]);
  is64 = dat["is64"] != "0";
  isARM = dat["isARM"] != "0";
  isServer = dat["isServer"] != "0";
  auto vs = Commons::splitStr(dat["versionStrings"], "\t");
  for(auto &v : vs)
    {
      if(v.size() > 0)
        {
          versionStrings.push_back(v);
        }
    }
}

JVMProfile::JVMProfile(){};

std::map<std::string, std::string>
JVMProfile::toMap()
{
  std::map<std::string, std::string> dat;
  dat["id"] = id;
  dat["bin"] = bin;
  dat["specVersion"] = std::to_string(specVersion);
  dat["is64"] = is64 ? std::string("1") : std::string("0");
  dat["isServer"] = isServer ? std::string("1") : std::string("0");
  dat["isARM"] = isARM ? std::string("1") : std::string("0");
  std::string vs;
  for(auto &v : versionStrings)
    {
      vs += (v + "\t");
    }
  dat["versionStrings"] = vs;
  return dat;
}

void
appendJVM(const std::string &bin, std::function<void(bool)> cb)
{
  // Read output
  LOG("Collecting JVM info from " << bin);
  Sys::runOnUVThread([=]() -> void {
    std::list<std::string> args;
    args.push_back("-XshowSettings:properties");
    Commons::runCommand(
        bin, args,
        [=](std::string settings, int) mutable -> void {
          args.push_back("-version");
          Commons::runCommand(
              bin, args,
              [=](std::string result, int) -> void {
                JVMProfile prof;
                if(result.size() == 0)
                  {
                    LOG("No valid JVM info from " << bin);
                    cb(false);
                    return;
                  }
                // Extract version info
                std::regex javaVersionRegex("[\"']([0-9._\\-a-z]+?)[\"']",
                                            std::regex_constants::icase);
                std::regex javaLegacyRegex("^1\\.([0-9])",
                                           std::regex_constants::icase);
                std::regex javaNewRegex("^([0-9]{2,})[.-]+?",
                                        std::regex_constants::icase);
                std::regex armRegex("os\\.arch *\\= *a(arch|rm)64");
                std::smatch match;
                if(std::regex_search(settings, armRegex))
                  {
                    prof.isARM = true;
                  }
                if(std::regex_search(result, match, javaVersionRegex))
                  {
                    std::string javaVersion = match[1];

                    // 1.0 - 1.9
                    if(std::regex_search(javaVersion, match, javaLegacyRegex))
                      {
                        // NOP
                      }
                    // 10+
                    else if(std::regex_search(javaVersion, match,
                                              javaNewRegex))
                      {
                        // NOP
                      }

                    std::string javaFinalVersion = match[1];
                    if(javaFinalVersion.size() > 0)
                      {
                        prof.specVersion = std::stoi(javaFinalVersion);
                      }
                    else
                      {
                        LOG("Could not detect version from " << bin);
                        cb(false);
                      }
                  }

                // Collect info
                auto lines = Commons::splitStr(result, "\n");
                for(auto &l : lines)
                  {
                    if(l.size() > 0)
                      {
                        prof.versionStrings.push_back(l);
                      }
                  }

                // Getting performance
                prof.is64 = !std::regex_search(
                    result, std::regex("32-bit", std::regex_constants::icase));
                prof.isServer = !std::regex_search(
                    result, std::regex("client", std::regex_constants::icase));

                // Executable
                prof.bin = bin;

                // A random one will be fine
                prof.id = Commons::genUUID();

                JVM_PROFILES.push_back(prof);
                LOG("Registered new JVM with version "
                    << prof.specVersion
                    << (prof.isARM ? " IS ARM" : " NOT ARM") << " from "
                    << bin);
                cb(true);
              },
              2);
        },
        2);
  });
}

std::vector<JVMProfile> JVM_PROFILES;
}
}