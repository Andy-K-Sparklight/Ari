#ifndef INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS
#define INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS

#include <map>
#include <vector>
#include <string>

namespace Alicorn
{
namespace Sys
{

extern std::map<std::string, std::string> ACH_CONFIG;

void loadConfig();

std::string getValue(const std::string &k, const std::string &dv);

void setValue(const std::string &k, const std::string &v);

void saveConfig();

// Load KV Group
std::vector<std::map<std::string, std::string>>
loadKVG(const std::string &rel);

// Save KV Group
void saveKVG(const std::string &rel,
             const std::vector<std::map<std::string, std::string>> &data);

template <typename T>
void
loadGlobalData(std::vector<T> &global, const std::string &name)
{
  auto dat = Sys::loadKVG(name);
  for(auto &s : dat)
    {
      T t(s);
      global.push_back(t);
    }
}

template <typename T>
void
saveGlobalData(std::vector<T> &global, const std::string &name)
{
  std::vector<std::map<std::string, std::string>> dat;
  for(auto &o : global)
    {
      dat.push_back(o.toMap());
    }
  Sys::saveKVG(name, dat);
}

}
}
#endif /* INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS */
