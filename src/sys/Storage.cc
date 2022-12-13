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

#define ACH_CONFIG_DIR ".achconf"
#define ACH_CONFIG_FILE "Main.kv"

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
  std::filesystem::create_directories(
      std::filesystem::path(getCfgPath(ACH_CONFIG_FILE)).parent_path());
}

void
loadConfig()
{
  auto f = getCfgPath(ACH_CONFIG_FILE);
  LOG("Loading config from " << f);
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

#define ACH_CFG_LINESPLIT "\n\t\a%%\a\t\n"
#define ACH_CFG_KVSPLIT "\t\a%$==$%\a\t"
#define ACH_CFG_SECSPLIT "\t\a\n==%MSIEKCU%==\n\a\t"

void
saveConfig()
{
  mkConfigDir();
  auto f = getCfgPath(ACH_CONFIG_FILE);
  LOG("Saving config to " << f);
  std::ofstream file(f);
  for(auto &p : ACH_CONFIG)
    {
      file << p.first << ACH_CFG_KVSPLIT << p.second << ACH_CFG_LINESPLIT;
    }
  file.close();
}

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