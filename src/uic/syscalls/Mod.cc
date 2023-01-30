#include "ach/uic/syscalls/Mod.hh"

#include "ach/extra/mod/provider/Modrinth.hh"
#include "ach/extra/mod/Modburin.hh"
#include "ach/core/platform/Tools.hh"
#include "ach/sys/Storage.hh"
#include "ach/core/profile/LaunchProfile.hh"
#include "ach/core/op/Flow.hh"
#include "ach/core/op/ProfileExt.hh"
#include "ach/uic/UserData.hh"
#include "ach/core/profile/GameProfile.hh"
#include <log.hh>

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void
implOpenModrinth(ACH_SC_ARGS)
{
  Extra::Mod::Modrinth::showModrinthWindow([cb]() -> void { cb(); });
}

void
implGetMods(ACH_SC_ARGS)
{
  using namespace Extra::Mod;
  auto metaRoot = getBurinBase() / "metas";
  prog.stack.push_back("");
  Platform::readDirAsync(
      metaRoot.string(),
      [&prog, cb, metaRoot](const std::list<std::string> &metas) -> void {
        LOG("Found " << metas.size() << " mods.");
        for(auto l : metas)
          {
            l.pop_back(); // Remove suffix
            auto meta = Sys::loadKVG((metaRoot / l).string());
            if(meta.size() != 0)
              {
                ModMeta mt(meta[0]);
                prog.stack.push_back(mt.icon);
                prog.stack.push_back(mt.slug);
                prog.stack.push_back(mt.displayName);
                prog.stack.push_back(mt.bid);
              }
          }
        cb();
      });
}

void
implConfigureMods(ACH_SC_ARGS)
{
  using namespace Extra::Mod;
  std::vector<std::string> mods;

  auto lpid = (*UIC::getUserData())["$Profile"];
  Profile::LaunchProfile *lpx = nullptr;
  for(auto &lp : Profile::LAUNCH_PROFILES)
    {
      if(lp.id == lpid)
        {
          lpx = &lp;
        }
    }
  if(lpx == nullptr)
    {
      LOG("Specified profile not found: " << lpid);
      prog.carry = false;
      cb();
      return;
    }
  Op::Flow *f = new Op::Flow;
  f->data[AL_FLOWVAR_PROFILEID] = lpx->baseProfile;
  f->addTask(Op::loadProfile);
  f->onProgress = ACH_DEFAULT_PROGRESS;
  f->onStep = ACH_DEFAULT_STEP;
  f->run([f, &prog, cb, lpid](bool s) -> void {
    if(s)
      {
        auto metaRoot = getBurinBase() / "metas";
        std::set<std::string> vers;
        std::set<std::string> modrinthDownloads;
        for(;;)
          {
            std::string cmodid = std::string(prog.stack.back());
            prog.stack.pop_back();
            if(cmodid.size() == 0)
              {
                break;
              }

            auto meta = Sys::loadKVG((metaRoot / cmodid).string());
            if(meta.size() != 0)
              {
                ModMeta mt(meta[0]);

                // Sync metas if necessary
                if(mt.provider == "MR")
                  {
                    Modrinth::syncModVersions(cmodid);
                  }
                else
                  {
                    LOG("Unsupported provider: " << mt.provider);
                    continue;
                  }

                // Pick version
                auto ld = Profile::canonicalLoader(f->profile.id);
                auto mcv = f->profile.baseVersion;
                auto v = pickVersion(cmodid, mcv, ld);
                if(v.size() > 0)
                  {
                    vers.insert(v);
                    if(mt.provider == "MR")
                      {
                        modrinthDownloads.insert(v);
                      }
                  }
                else
                  {
                    LOG("Could not pick proper version for "
                        << mt.slug << " with game " << mcv << " and loader "
                        << ld);
                  }
              }
          }
        Modrinth::dlModVersion(modrinthDownloads,
                               [&prog, lpid, vers, cb](bool s) -> void {
                                 if(collectVersions(vers, lpid))
                                   {
                                     LOG("Completed mods installing.");
                                     prog.carry = true;
                                   }
                                 else
                                   {
                                     LOG("Some mods failed to install!");
                                     prog.carry = false;
                                   }
                                 cb();
                               });
      }
    else
      {
        prog.carry = false;
        cb();
      }
    delete f;
  });
}

}
}
}