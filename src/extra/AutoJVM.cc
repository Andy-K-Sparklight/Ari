#include "ach/extra/AutoJVM.hh"

#include "ach/util/Proc.hh"
#include "ach/util/Commons.hh"
#include "ach/core/platform/OSType.hh"
#include "ach/core/platform/Finder.hh"
#include "ach/core/network/Download.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/platform/Tools.hh"
#include <filesystem>

#include <log.hh>

namespace Alicorn
{
namespace Extra
{

void
scanJVM(std::function<void(std::list<std::string>)> cb)
{
  LOG("Scanning for JVMs.");
  std::list<std::string> args;
  std::string bin;
  if(Platform::OS_TYPE == Platform::OS_MSDOS)
    {
      bin = "where";
    }
  else
    {
      bin = "which";
      args.push_back("-a");
    }
  args.push_back("java");
  Commons::runCommand(
      bin, args,
      [=](std::string output, int) -> void {
        std::list<std::string> jvms;
        auto dat = Commons::splitStr(output, "\n");
        for(auto &b : dat)
          {
            if(b.ends_with("\r"))
              {
                b.pop_back();
              }
            if(b.size() > 0)
              {
                jvms.push_back(b);
              }
          };
        auto jvmPath8 = Platform::getJVMPath("8");
        auto jvmPath18 = Platform::getJVMPath("18");
        if(std::filesystem::exists(jvmPath8))
          {
            jvms.push_front(jvmPath8);
          }
        if(std::filesystem::exists(jvmPath18))
          {
            jvms.push_front(jvmPath18);
          }
        LOG(jvms.size() << " JVMs found.");
        cb(jvms);
      },
      1);
}

#define ACH_JVM_DL_ROOT                                                       \
  "https://github.com/Andy-K-Sparklight/MCJREBinaries/releases/download/1.0/"
#define ACH_JVM_DL_WIN32 "win32.zip"
#define ACH_JVM_DL_OSX "osx.zip"
#define ACH_JVM_DL_OSXARM "osxarm.zip"
#define ACH_JVM_DL_GNU "gnu.zip"

void
installJVM(std::function<void(bool)> cb)
{
  std::string dlUrl = ACH_JVM_DL_ROOT;
  if(Platform::OS_TYPE == Platform::OS_MSDOS)
    {
      dlUrl += ACH_JVM_DL_WIN32;
    }
  else if(Platform::OS_TYPE == Platform::OS_DARWIN)
    {
      if(Platform::OS_ARCH == Platform::AR_ARM)
        {
          dlUrl += ACH_JVM_DL_OSXARM;
        }
      else if(Platform::OS_ARCH == Platform::AR_X86)
        {
          dlUrl += ACH_JVM_DL_OSX;
        }
      else
        {
          LOG("Cannot automate JVM installing for this platform!");
          cb(false);
          return;
        }
    }
  else if(Platform::OS_TYPE == Platform::OS_UNIX)
    {
      dlUrl += ACH_JVM_DL_GNU;
    }
  else
    {
      LOG("Cannot automate JVM installing for this platform!");
      cb(false);
      return;
    }
  LOG("Configured JVM url: " << dlUrl);
  Network::DownloadPack pk;
  Network::DownloadMeta meta;
  meta.baseURL = dlUrl;
  meta.path = Platform::getJVMHomePath() + "/bundle.zip";
  pk.addTask(meta);
  pk.assignUpdateCallback([=](const Network::DownloadPack *pack) -> void {
    auto stat = pack->getStat();
    if(stat.completed + stat.failed == stat.total)
      {
        if(stat.failed == 0)
          {
            LOG("Unpacking JVM packages.");
            if(Platform::unzipFile(meta.path, Platform::getJVMHomePath()))
              {
                std::filesystem::remove(meta.path);
                LOG("Verifying JVM installation.")
                if(std::filesystem::exists(Platform::getJVMPath("18")))
                  {
                    LOG("Verified JVM installation. Done.");
                    cb(true);
                  }
                else
                  {
                    LOG("Could not verify JVM installation!");
                    try
                      {
                        std::filesystem::remove_all(
                            Platform::getJVMHomePath());
                      }
                    catch(std::exception &ignored)
                      {
                      }
                    cb(false);
                  };
              }
            else
              {
                LOG("Could not unzip JVM!");
                cb(false);
              }
          }
        else
          {
            LOG("Could not download JVM!");
            cb(false);
          }
      }
  });
  Sys::runOnWorkerThread([=]() mutable -> void {
    LOG("Downloading JVM to " << meta.path);
    pk.commit();
    Network::ALL_DOWNLOADS.push_back(pk);
  });
}
}
}