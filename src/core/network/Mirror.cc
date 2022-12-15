#include "ach/core/network/Mirror.hh"

#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Network
{

// Predifined, usually won't need to change
#define ACH_MIRROR_RULES                                                      \
  "piston-meta.mojang.com:download.mcbbs.net;"                                \
  "launcher.mojang.com:download.mcbbs.net;"                                   \
  "launchermeta.mojang.com:download.mcbbs.net;"                               \
  "resources.download.minecraft.net:download.mcbbs.net/assets;"               \
  "libraries.minecraft.net:download.mcbbs.net/maven;"                         \
  "files.minecraftforge.net/maven:download.mcbbs.net/maven;"                  \
  "meta.fabricmc.net:download.mcbbs.net/fabric-meta;"                         \
  "maven.fabricmc.net:download.mcbbs.net/maven;"                              \
  ".:."

static std::vector<std::pair<std::string, std::string>> mirrorRules;

std::vector<std::string>
getMirrors(const std::string &source)
{
  if(mirrorRules.size() == 0)
    {
      auto rs = Commons::splitStr(ACH_MIRROR_RULES, ";");
      for(auto &r : rs)
        {
          auto kv = Commons::splitStr(r, ":");
          if(kv.size() == 2)
            {
              mirrorRules.push_back({ kv[0], kv[1] });
            }
        }
    }
  std::vector<std::string> resultURLS;
  for(auto &r : mirrorRules)
    {
      if(source.contains(r.first))
        {
          size_t pos = source.find(r.first);
          if(pos != std::string::npos)
            {
              std::string replStr = source;
              replStr.replace(pos, r.first.size(), r.second);
              resultURLS.push_back(replStr);
            }
        }
    }
  return resultURLS;
}

}
}