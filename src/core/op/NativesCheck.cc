#include "ach/core/op/NativesCheck.hh"

#include <string>
#include <filesystem>
#include <list>
#include "ach/core/platform/Tools.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/platform/Finder.hh"
#include "ach/core/profile/GameProfile.hh"
#include "ach/util/Commons.hh"
#include <iostream>
#include <fstream>
#include <log.hh>

namespace Alicorn
{
namespace Op
{

void
collectNatives(Flow *flow, FlowCallback cb)
{
  using namespace Platform;
  cb(AL_UNPACKNAT);
  Sys::runOnWorkerThread([=]() -> void {
    Profile::VersionProfile profile = flow->profile;
    LOG("Collecting native libraries for " << profile.id);
    auto base = getStoragePath("libraries");
    auto target = getStoragePath("versions/" + profile.id
                                 + "/.natives"); // Direct install
    auto unpackwd = getTempPath("unpack/" + profile.id);
    std::filesystem::create_directories(unpackwd);
    std::filesystem::create_directories(target);
    int natives = 0;
    for(auto &l : profile.libraries)
      {
        if(l.isNative)
          {
            natives++;
            auto libJarPath = Commons::normalizePath(
                std::filesystem::path(base) / l.artifact.path);
            unzipFile(libJarPath, unpackwd);
          }
      }
    LOG("Unpacked " << natives << " native libraries.");
    cb(AL_COLNAT);
    auto files = scanDirectory(unpackwd);
    natives = 0;
    for(auto &f : files)
      {
        if(f.ends_with(".dll") || f.ends_with(".so") || f.ends_with(".dylib"))
          {
            // Use the sync method to avoid any conflict
            try
              {
                auto dest = std::filesystem::path(target)
                            / std::filesystem::path(f).filename();
                Platform::mkParentDirs(dest);
                if(!std::filesystem::exists(dest))
                  {
                    std::filesystem::copy_file(f, dest);
                  }
                natives++;
              }
            catch(std::exception &ignored)
              {
              }
          }
      }
    LOG("Collected " << natives << " extracted native libraries.");
    std::filesystem::remove_all(unpackwd);
    cb(AL_OK);
  });
}

}
}