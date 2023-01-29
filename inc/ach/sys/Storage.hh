#ifndef INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS
#define INCLUDE_ALICORN_CH_ACH_SYS_OPTIONS

#include <map>
#include <vector>
#include <string>

#define ACH_CFG_DLM "dlMirror"
#define ACH_CFG_DLM_ACC "Accept"

namespace Alicorn
{
namespace Sys
{

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
