#include "ach/core/op/NativesCheck.hh"

#include <string>
#include <filesystem>
#include <list>
#include "ach/core/op/Tools.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Finder.hh"
#include <iostream>
#include <fstream>

namespace Alicorn
{
namespace Op
{

void
checkNatives(Flow *flow, FlowCallback cb)
{
  auto pt = getStoragePath("libraries");
  auto targetPt = getInstallPath("libraries");
  cb(AL_SCANNAT);
  std::list<std::string> retLibs = scanDirectory(pt);
  Sys::runOnWorkerThread([=]() -> void {
    cb(AL_UNPACKNAT);
    bool allPassed = true;
    for(auto &l : retLibs)
      {
        auto fileName = std::filesystem::path(l).filename().string();
        // We changed the .jar to .zip, to identify native
        if(fileName.ends_with(".jarn"))
          {
            auto currentPt = std::filesystem::path(l).replace_extension("");
            auto unzipPrefix = std::filesystem::path(targetPt)
                               / std::filesystem::relative(currentPt, pt);
            if(!unzipFile(l, unzipPrefix.string()))
              {
                allPassed = false;
              }
          }
      }
    cb(allPassed ? AL_OK : AL_ERR);
  });
}

}
}