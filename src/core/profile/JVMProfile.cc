#include "ach/core/profile/JVMProfile.hh"

#include "ach/sys/Storage.hh"
#include "ach/util/Commons.hh"
#include "ach/sys/Schedule.hh"
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <regex>
#include <uuid.h>

namespace Alicorn
{
namespace Profile
{

#define ACH_JVM_PROFILE "JVMProf.kvg"

JVMProfile::JVMProfile(const std::map<std::string, std::string> &slice)
{
  std::map<std::string, std::string> dat = slice;
  id = dat["id"];
  bin = dat["bin"];
  specVersion = std::stoi(dat["specVersion"]);
  is64 = dat["is64"] != "0";
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
  dat["is64"] = is64 ? "1" : "0";
  dat["isServer"] = isServer ? "1" : "0";
  std::string vs;
  for(auto &v : versionStrings)
    {
      vs += (v + "\t");
    }
  dat["versionStrings"] = vs;
  return dat;
}

void
loadJVMProfiles()
{
  auto profiles = Sys::loadKVG(ACH_JVM_PROFILE);
  for(auto &p : profiles)
    {
      JVMProfile pr(p);
      JVM_PROFILES.push_back(pr);
    }
}

void
saveJVMProfiles()
{
  std::vector<std::map<std::string, std::string> > vec;
  for(auto &p : JVM_PROFILES)
    {
      vec.push_back(p.toMap());
    }
  Sys::saveKVG(ACH_JVM_PROFILE, vec);
}

bool
appendJVM(const std::string &bin)
{
  JVMProfile prof;

  // Read output
  std::string cmd = "\"" + bin + "\" -version 2>&1";
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "rb"),
                                                pclose);
  if(!pipe)
    {
      return false;
    }
  while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
      result += buffer.data();
    }

  // Extract version info
  std::regex javaVersionRegex("[\"']([0-9._\\-a-z]+?)[\"']",
                              std::regex_constants::icase);
  std::regex javaLegacyRegex("^1\\.([0-9])", std::regex_constants::icase);
  std::regex javaNewRegex("^([0-9]{2,})[.-]+?", std::regex_constants::icase);
  std::smatch match;
  if(std::regex_search(result, match, javaVersionRegex))
    {
      std::string javaVersion = match[1];

      // 1.0 - 1.9
      if(std::regex_search(javaVersion, match, javaLegacyRegex))
        {
        }
      // 10+
      else if(std::regex_search(javaVersion, match, javaNewRegex))
        {
        }

      std::string javaFinalVersion = match[1];
      if(javaFinalVersion.size() > 0)
        {
          prof.specVersion = std::stoi(javaFinalVersion);
        }
      else
        {
          return false;
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

  return true;
}

std::vector<JVMProfile> JVM_PROFILES;
}
}