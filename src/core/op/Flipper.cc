#include "ach/core/op/Flipper.hh"

#include "ach/core/op/Tools.hh"
#include "ach/core/op/Finder.hh"
#include "ach/sys/Schedule.hh"

#include <iostream>
#include <filesystem>

namespace Alicorn
{
namespace Op
{

void
flipInstall(Flow *flow, FlowCallback cb)
{
  Sys::runOnWorkerThread([=]() -> void {
    cb(AL_FLIPDIR);
    auto base = getInstallPath(".");
    std::list<std::string> files = scanDirectory(base);
    int *total = new int(files.size());
    int *count = new int(0);
    for(auto &f : files)
      {
        auto target = getStoragePath(std::filesystem::relative(f, base));
        mkParentDirs(target);
        copyFileAsync(f, target, [=](bool stat) -> void {
          (*count)++;
          if(*count == *total)
            {
              delete count;
              delete total;
              Sys::runOnWorkerThread([=]() -> void {
                std::filesystem::remove_all(base); // Delete source
                cb(AL_OK);
              });
            }
        });
      }
  });
}

}
}