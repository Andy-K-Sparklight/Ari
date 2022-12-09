#include "ach/sys/Options.hh"

#include <filesystem>
#include <fstream>
#include <sstream>
#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Sys
{

#define ACH_CONFIG_FILE ".ach.kv"

std::map<std::string, std::string> ACH_CONFIG;

static std::string
getCfgPath()
{
  char *base;
  if((base = getenv("HOME")) != NULL)
    {
    }
  else if((base = getenv("USERPROFILE")) != NULL)
    {
    }
  if(base != NULL)
    {
      return (std::filesystem::path(std::string(base)) / ACH_CONFIG_FILE)
          .string();
    }
  else
    {
      return ACH_CONFIG_FILE; // Hopefully not local...
    }
}

void
loadConfig()
{
  auto f = getCfgPath();
  std::ifstream file(f);
  std::stringstream ss;
  ss << file.rdbuf();
  auto s = ss.str();
  auto splits = Commons::splitStr(s, "\n\n");
  for(auto &l : splits)
    {
      auto kv = Commons::splitStr(l, "\n");
      if(kv.size() == 2)
        {
          ACH_CONFIG[kv[0]] = kv[1];
        }
    }
  file.close();
}

std::string
getValue(const std::string &k, const std::string &dv)
{
  auto s = ACH_CONFIG[k];
  if(s.size() == 0)
    {
      return dv;
    }
  return s;
}

void
setValue(const std::string &k, const std::string &v)
{
  ACH_CONFIG[k] = v;
}

void
saveConfig()
{
  auto f = getCfgPath();
  std::ofstream file(f);
  for(auto &p : ACH_CONFIG)
    {
      file << p.first << "\n" << p.second << "\n\n";
    }
  file.close();
}

}
}