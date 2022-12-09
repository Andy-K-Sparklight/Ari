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
    auto base = getStoragePath("libraries");
    auto target = getStoragePath("versions/" + profile.id + "/.natives");
    auto unpackwd = getTempPath("unpack/" + profile.id);
    std::filesystem::create_directories(unpackwd);
    std::filesystem::create_directories(target);
    for(auto &l : profile.libraries)
      {
        if(l.isNative)
          {
            auto libJarPath = Commons::normalizePath(
                std::filesystem::path(base) / l.artifact.path);
            unzipFile(libJarPath, unpackwd);
          }
      }
    cb(AL_COLNAT);
    auto files = scanDirectory(unpackwd);
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
              }
            catch(std::exception &ignored)
              {
              }
          }
      }
    std::filesystem::remove_all(unpackwd);
    cb(AL_OK);
  });
}

}
}