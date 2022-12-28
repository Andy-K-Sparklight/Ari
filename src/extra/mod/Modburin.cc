#include "ach/extra/mod/Modburin.hh"

#include "ach/util/Commons.hh"
#include "ach/core/platform/Finder.hh"
#include <fstream>
#include "ach/core/platform/Tools.hh"
#include "ach/sys/Storage.hh"
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
          for(size_t i = 0; i < (sizeof(ACH_BURIN_SPLIT) / sizeof(char)) - 1;
              i++) // -1 because sizeof includes NULL at the end
            {
              src.pop_back();
            }
        }
    }
}

std::filesystem::path
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
  provider = dat["provider"];
  mid = dat["mid"];
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
  dat["provider"] = provider;
  dat["mid"] = mid;
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
      auto src = getBurinBase() / "files" / v;
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
  auto localModsRoot = getBurinBase() / "dynamics" / lpid;
  auto remoteModsRoot = Platform::getRuntimePath(lpid + "/mods");
  // It's managed by us
  if(std::filesystem::exists(remoteModsRoot))
    {
      try
        {
          std::filesystem::remove_all(remoteModsRoot);
        }
      catch(std::exception &e)
        {
        }
    }
  LOG("Linking mod directory: " << remoteModsRoot << " <- " << localModsRoot);
  try
    {
      std::filesystem::create_hard_link(localModsRoot, remoteModsRoot);
    }
  catch(std::exception &e)
    {
      LOG("Failed to create hard link for mod directory, using symlink "
          "instead.");
      try
        {
          std::filesystem::create_symlink(localModsRoot, remoteModsRoot);
        }
      catch(std::exception &e)
        {
          LOG("Could not link mod directory.");
          return false;
        }
    }
  LOG("Finished linking mod directory.");
  return true;
}

bool
addVersionFromFile(const std::string &filePt)
{
  ModVersion vs;
  vs.bid = Commons::genUUID();
  vs.provider = "LOCAL";
  auto fname = std::filesystem::path(filePt).filename().string();
  vs.slug = fname;
  LOG("Creating new version " << vs.bid << " from " << filePt);
  auto target = getBurinBase() / "files" / vs.bid / fname;
  auto metaTarget = getBurinBase() / "versions" / vs.bid;
  Platform::mkParentDirs(target);
  Platform::mkParentDirs(target);
  try
    {
      auto mmap = vs.toMap();
      Sys::saveKVG(metaTarget.string(), { mmap });
      std::filesystem::copy_file(filePt, target);
    }
  catch(std::exception &e)
    {
      LOG("Something went wrong when creating: " << e.what());
      return false;
    }
  LOG("Created new version " << vs.bid);
  return true;
}

std::string
pickVersion(const std::string &mid, const std::string &gv,
            const std::string &ld)
{
  LOG("Picking version for " << mid << " with game version " << gv
                             << " and loader " << ld);
  auto target = getBurinBase() / "metas" / mid;
  auto meta = Sys::loadKVG(target.string());
  if(meta.size() == 0)
    {
      return "";
    }
  ModMeta mt(meta[0]);
  for(auto &m : mt.versions)
    {
      auto uuid = Commons::genUUID(m);
      auto vTarget = getBurinBase() / "versions" / uuid;
      auto vMeta = Sys::loadKVG(vTarget.string());
      if(vMeta.size() > 0)
        {
          ModVersion mv(vMeta[0]);
          if(mv.loaders.contains(ld) && mv.gameVersions.contains(gv))
            {
              return mv.bid;
            }
        }
    }
  return "";
}

}
}
}