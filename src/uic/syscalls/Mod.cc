#include "ach/uic/syscalls/Mod.hh"

#include "ach/extra/mod/provider/Modrinth.hh"
#include "ach/extra/mod/Modburin.hh"
#include "ach/core/platform/Tools.hh"
#include "ach/sys/Storage.hh"
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

}
}
}