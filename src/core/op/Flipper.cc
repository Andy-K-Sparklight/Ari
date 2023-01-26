#include "ach/core/op/Flipper.hh"

#include "ach/core/platform/Tools.hh"
#include "ach/core/platform/Finder.hh"
#include "ach/sys/Schedule.hh"
#include "ach/util/Commons.hh"

#include <iostream>
#include <filesystem>
#include <log.hh>

namespace Alicorn
{
namespace Op
{

void
flipInstall(Flow *flow, FlowCallback cb)
{
  using namespace Platform;
  cb(AL_FLIPDIR);
  Sys::runOnWorkerThread([=]() -> void {
    auto base = getInstallPath();
    std::list<std::string> files = scanDirectory(base);
    if(files.size() == 0)
      {
        cb(AL_OK);
        return;
      }
    LOG("Flipping dir " << base);

    // Run sync ops first
    for(auto &f : files)
      {
        auto target = getStoragePath(std::filesystem::relative(f, base));
        mkParentDirs(target);
      }

    int *total = new int(files.size());
    int *count = new int(0);
    Sys::runOnUVThread([=]() -> void {
      for(auto &f : files)
        {
          auto target = getStoragePath(std::filesystem::relative(f, base));
          moveFileAsync(f, target, [=](bool stat) -> void {
            (*count)++;
            if(*count == *total)
              {
                LOG("Successfully flipped " << *count << " files in " << base);
                delete count;
                delete total;
                Sys::runOnWorkerThread([=]() -> void {
                  try
                    {
                      std::filesystem::remove_all(base); // Delete source
                      cb(AL_OK);
                    }
                  catch(std::exception &e)
                    {
                      cb(AL_ERR);
                    }
                });
              }
          });
        }
    });
  });
}

}
}