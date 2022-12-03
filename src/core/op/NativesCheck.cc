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

static void
collectFiles(const std::filesystem::path pt,
             std::function<void(std::list<std::string>)> cb,
             std::function<bool(std::string)> judge,
             std::list<std::string> *pointer = nullptr)
{
  std::list<std::string> *currentRet = pointer;
  if(pointer == nullptr)
    {
      currentRet = new std::list<std::string>;
    }
  readDirAsync(pt.string(), [=](std::list<std::string> ents) -> void {
    int *total = new int(0);
    int *collected = new int(0);
    for(auto &e : ents)
      {
        if(e.ends_with("D"))
          {
            (*total)++;
          }
        else if(e.ends_with("F"))
          {
            std::string s = e;
            s.pop_back();
            std::string spt = (pt / s).string();
            if(judge(spt))
              {
                currentRet->push_back(spt);
              }
          }
      }
    if(*total == 0)
      {
        cb((std::list<std::string>)0);
        delete collected;
        delete total;
        return;
      }
    for(auto &e : ents)
      {
        if(e.ends_with("D"))
          {
            e.pop_back();
            std::cout << "Collecting: " << e << std::endl;
            collectFiles(
                pt / e,
                [=](std::list<std::string> dat) -> void {
                  (*collected)++;
                  if((*collected) == (*total))
                    {
                      std::list<std::string> a;
                      if(pointer == nullptr)
                        {
                          a = *currentRet;
                        }
                      cb(a);
                      delete collected;
                      delete total;
                    }
                },
                judge, currentRet);
          }
      }
  });
}

void
checkNatives(Flow *flow, FlowCallback cb)
{
  auto pt = getStoragePath("libraries");
  auto targetPt = getInstallPath("libraries");
  cb(AL_SCANNAT);
  collectFiles(
      pt,
      [=](std::list<std::string> retLibs) -> void {
        Sys::runOnWorkerThread([=]() -> void {
          cb(AL_UNPACKNAT);
          bool allPassed = true;
          for(auto &l : retLibs)
            {
              auto currentPt = std::filesystem::path(l).replace_extension("");
              auto unzipPrefix = std::filesystem::path(targetPt)
                                 / std::filesystem::relative(currentPt, pt);

              if(!unzipFile(l, unzipPrefix.string()))
                {
                  allPassed = false;
                }
            }
          cb(allPassed ? AL_OK : AL_ERR);
        });
      },
      [](std::string s) -> bool {
        auto fileName = std::filesystem::path(s).filename().string();
        return fileName.contains("natives-linux")
               || fileName.contains("natives-windows")
               || fileName.contains("natives-macos");
      });
}

}
}