#include "ach/extra/mod/Modburin.hh"

#include "ach/util/Commons.hh"
#include "ach/core/platform/Finder.hh"
#include <filesystem>
#include "ach/core/platform/Tools.hh"
#include <log.hh>

namespace Alicorn
{
namespace Extra
{
namespace Mod
{

#define ACH_BURIN_SPLIT ",\x12\x06\x12,"

static void
strset(std::set<std::string> &s, std::string &src, bool write)
{
  if(write)
    {
      s.clear();
      auto p = Commons::splitStr(src, ACH_BURIN_SPLIT);
      for(auto &l : p)
        {
          s.insert(l);
        }
    }
  else
    {
      src.clear();
      for(auto &l : s)
        {
          src += (l + ACH_BURIN_SPLIT);
        }
      if(src.size() > 0)
        {
          for(size_t i = 0; i < sizeof(ACH_BURIN_SPLIT) / sizeof(char); i++)
            {
              src.pop_back();
            }
        }
    }
}

static std::filesystem::path
getBurinBase()
{
  return std::filesystem::path(Platform::getBasePath()) / "burin";
}

ModMeta::ModMeta(std::map<std::string, std::string> &slice)
{
  auto dat = slice;
  bid = dat["bid"];
  displayName = dat["displayName"];
  desc = dat["desc"];
  pid = dat["pid"];
  slug = dat["slug"];
  provider = dat["provider"];
  strset(versions, dat["versions"], true);
  icon = dat["icon"];
}

ModMeta::ModMeta(){};

std::map<std::string, std::string>
ModMeta::toMap()
{
  std::map<std::string, std::string> dat;
  dat["bid"] = bid;
  dat["displayName"] = displayName;
  dat["desc"] = desc;
  dat["pid"] = pid;
  dat["slug"] = slug;
  dat["provider"] = provider;
  strset(versions, dat["versions"], false);
  dat["icon"] = icon;
  return dat;
}

ModVersion::ModVersion(std::map<std::string, std::string> &slice)
{
  auto dat = slice;
  bid = dat["bid"];
  displayName = dat["displayName"];
  pid = dat["pid"];
  slug = dat["slug"];
  strset(urls, dat["urls"], true);
  strset(gameVersions, dat["gameVersions"], true);
  strset(loaders, dat["loaders"], true);
}

ModVersion::ModVersion(){};

std::map<std::string, std::string>
ModVersion::toMap()
{
  std::map<std::string, std::string> dat;
  dat["bid"] = bid;
  dat["displayName"] = displayName;
  dat["pid"] = pid;
  dat["slug"] = slug;
  strset(urls, dat["urls"], false);
  strset(gameVersions, dat["gameVersions"], false);
  strset(loaders, dat["loaders"], false);
  return dat;
}

bool
collectVersions(const std::set<std::string> vers, std::string lpid)
{
  LOG("Collecting mods for " << lpid);
  for(auto &v : vers)
    {
      auto src = getBurinBase() / "versions" / v;
      auto files = Platform::scanDirectory(src.string());
      for(auto &f : files)
        {
          auto rel = std::filesystem::relative(f, src);
          auto target = getBurinBase() / "dynamics" / lpid / rel;
          Platform::mkParentDirs(target);
          try
            {
              std::filesystem::copy_file(f, target);
            }
          catch(std::exception &e)
            {
              LOG("Something went wrong when collecting: " << e.what());
              return false;
            }
        }
    }
  LOG("Collected " << vers.size() << " mod files.");
  return true;
}

}
}
}