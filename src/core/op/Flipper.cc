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

    std::list<std::string> filesOp;

    // Run sync ops first
    for(auto &f : files)
      {
        auto target = getStoragePath(std::filesystem::relative(f, base));
        auto originSz = Commons::getFileSize(f);
        auto targetSz = Commons::getFileSize(target);
        if(targetSz > 0 && originSz == targetSz)
          {
            // Skip if exists with the same size
            continue;
          }
        mkParentDirs(target);
        filesOp.push_back(f);
      }

    int *total = new int(filesOp.size());
    int *count = new int(0);
    Sys::runOnUVThread([=]() -> void {
      for(auto &f : filesOp)
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