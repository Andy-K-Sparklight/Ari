#include "ach/sys/Storage.hh"

#include <filesystem>
#include <fstream>
#include <sstream>
#include "ach/util/Commons.hh"
#include <log.hh>

namespace Alicorn
{
namespace Sys
{

#define ACH_CONFIG_DIR ".arimc"

std::map<std::string, std::string> ACH_CONFIG;

static std::string
getCfgPath(const std::string &name)
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
      return (std::filesystem::path(std::string(base)) / ACH_CONFIG_DIR / name)
          .string();
    }
  else
    {
      return (std::filesystem::path(ACH_CONFIG_DIR) / name)
          .string(); // Hopefully not local...
    }
}

static void
mkConfigDir()
{
  std::filesystem::create_directories(std::filesystem::path(getCfgPath(".")));
}

#define ACH_CFG_LINESPLIT "\n\t\a\x06%LN%\x06\a\t\n"
#define ACH_CFG_KVSPLIT "\t\a\x11==\x11\a\t"
#define ACH_CFG_SECSPLIT "\t\a\n\x16=%MSIEKCU%=\x16\n\a\t"

std::vector<std::map<std::string, std::string>>
loadKVG(const std::string &rel)
{
  std::vector<std::map<std::string, std::string>> out;
  auto f = getCfgPath(rel);
  std::ifstream file(f);
  std::stringstream ss;
  ss << file.rdbuf();
  auto s = ss.str();
  auto splits = Commons::splitStr(s, ACH_CFG_SECSPLIT);
  for(auto &l : splits)
    {
      if(l.size() == 0)
        {
          continue;
        }
      std::map<std::string, std::string> o;
      auto lines = Commons::splitStr(l, ACH_CFG_LINESPLIT);
      for(auto &ln : lines)
        {
          if(ln.size() == 0)
            {
              continue;
            }
          auto kv = Commons::splitStr(ln, ACH_CFG_KVSPLIT);
          if(kv.size() == 2)
            {
              o[kv[0]] = kv[1];
            }
        }
      out.push_back(o);
    }
  file.close();
  return out;
}

void
saveKVG(const std::string &rel,
        const std::vector<std::map<std::string, std::string>> &data)
{
  mkConfigDir();
  auto f = getCfgPath(rel);
  std::ofstream file(f);
  for(auto &m : data)
    {
      for(auto &kv : m)
        {
          file << kv.first << ACH_CFG_KVSPLIT << kv.second
               << ACH_CFG_LINESPLIT;
        }
      file << ACH_CFG_SECSPLIT;
    }
  file.close();
}

}
}