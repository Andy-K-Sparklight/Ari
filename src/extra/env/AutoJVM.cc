#include "ach/extra/env/AutoJVM.hh"

#include "ach/util/Proc.hh"
#include "ach/util/Commons.hh"
#include "ach/core/platform/OSType.hh"
#include "ach/core/platform/Finder.hh"
#include "ach/core/network/Download.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/platform/Tools.hh"
#include "ach/core/profile/JVMProfile.hh"
#include <set>
#include <filesystem>
#include <unistd.h>

#include <log.hh>

namespace Alicorn
{
namespace Extra
{

namespace Env
{
static void
optionalAdd(std::set<std::string> &jvms, std::string b)
{
  if(b.ends_with("\r"))
    {
      b.pop_back();
    }
  b = Commons::normalizePath(b);
  auto stat = std::filesystem::status(b).type();
  if(stat == std::filesystem::file_type::regular)
    {
      jvms.insert(b);
    }
  else if(stat == std::filesystem::file_type::symlink)
    {
      auto origin = Commons::normalizePath(std::filesystem::read_symlink(b));
      if(!jvms.contains(origin))
        {
          jvms.insert(origin);
        }
    }
}

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
        // Do not block UV
        Sys::runOnWorkerThread([=]() -> void {
          std::set<std::string> jvms;
          auto dat = Commons::splitStr(output, "\n");
          for(auto b : dat)
            {
              optionalAdd(jvms, b);
            };
          // Let's also search for some specified directories
          std::list<std::string> extraJVMs;
          if(Platform::OS_TYPE == Platform::OS_DARWIN)
            {
              auto e = Platform::scanDirectory(
                  "/Library/Java/JavaVirtualMachines/");
              for(auto &p : e)
                {
                  if(p.ends_with("/java"))
                    {
                      extraJVMs.push_back(p);
                    }
                }
            }
          else if(Platform::OS_TYPE == Platform::OS_UNIX)
            {
              auto e = Platform::scanDirectory("/usr/lib/jvm/");
              for(auto &p : e)
                {
                  if(p.ends_with("/java"))
                    {
                      extraJVMs.push_back(p);
                    }
                }
            }
          else if(Platform::OS_TYPE == Platform::OS_MSDOS)
            {
              auto e1 = Platform::scanDirectory(
                  "C:\\Program Files\\Eclipse Adoptium");
              auto e2 = Platform::scanDirectory("C:\\Program Files\\Java");
              for(auto &p : e1)
                {
                  if(p.ends_with("\\java.exe"))
                    {
                      extraJVMs.push_back(p);
                    }
                }
              for(auto &p : e2)
                {
                  if(p.ends_with("java.exe"))
                    {
                      extraJVMs.push_back(p);
                    }
                }
            }
          for(auto b : extraJVMs)
            {
              optionalAdd(jvms, b);
            }
          // Summarize
          std::list<std::string> outJVMs;
          for(auto &p : jvms)
            {
              outJVMs.push_back(p);
            }
          // Our JVM
          auto jvmPath8 = Platform::getJVMPath("8");
          auto jvmPath18 = Platform::getJVMPath("18");
          if(std::filesystem::exists(jvmPath8))
            {
              outJVMs.push_front(jvmPath8);
            }
          if(std::filesystem::exists(jvmPath18))
            {
              outJVMs.push_front(jvmPath18);
            }
          LOG(outJVMs.size() << " JVMs found.");
          cb(outJVMs);
        });
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
installJVM(std::function<void(bool)> cb, Op::FlowProgress pg)
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
    pg((double)stat.completedSize / stat.totalSize);
    if(stat.completed + stat.failed == stat.total)
      {
        if(stat.failed == 0)
          {
            LOG("Unpacking JVM packages.");
            if(Platform::unzipFile(meta.path, Platform::getJVMHomePath()))
              {
                std::filesystem::remove(meta.path);
                LOG("Verifying JVM installation.")
                auto pt18 = Platform::getJVMPath("18");
                auto pt8 = Platform::getJVMPath("8");
                if(std::filesystem::exists(pt18))
                  {
                    if(Platform::OS_TYPE != Platform::OS_MSDOS)
                      {
                        chmod(pt18.c_str(), 0755);
                        if(std::filesystem::exists(pt8))
                          {
                            chmod(pt8.c_str(), 0755);
                          }
                      }
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
  });
}

void
configureJVM(Op::Flow *flow, Op::FlowCallback cb)
{
  cb(AL_DLJVM);
  LOG("Configuring JVM installation.");

  auto anyCb = [=](bool stat) -> void {
    LOG("Scanning for JVM installation.");
    cb(AL_SCANJVM);
    scanJVM([=](std::list<std::string> jvms) -> void {
      int *completed = new int(0);
      int *total = new int(jvms.size());
      for(auto &j : jvms)
        {
          Profile::JVM_PROFILES.clear();
          Sys::runOnUVThread([=]() -> void {
            Profile::appendJVM(j, [=](bool) -> void {
              (*completed)++;
              if(*completed == *total)
                {
                  LOG("Settled " << *total << " JVMs.");
                  delete completed;
                  delete total;
                  cb(AL_OK);
                }
            });
          });
        }
    });
  };
  if(flow->data[AL_FLOWVAR_DLJVM] == "0")
    {
      Sys::runOnWorkerThread([=]() -> void { anyCb(true); });
    }
  else
    {
      installJVM(anyCb, flow->onProgress);
    }
}

}
}
}