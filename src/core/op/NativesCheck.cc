#include "ach/core/op/NativesCheck.hh"

#include <string>
#include <filesystem>
#include <list>
#include "ach/core/op/Tools.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Finder.hh"
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
  cb(AL_UNPACKNAT);
  auto profileSrc = flow->data[AL_FLOWVAR_PROFILESRC];
  if(profileSrc.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  Sys::runOnWorkerThread([=]() -> void {
    Profile::VersionProfile profile(profileSrc);
    LOG("Collecting native libraries for " << profile.id);
    auto base = getStoragePath("libraries");
    auto target = getStoragePath("versions/" + profile.id + "/.natives");
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
                std::filesystem::copy_file(
                    f, std::filesystem::path(target)
                           / std::filesystem::path(f).filename());
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