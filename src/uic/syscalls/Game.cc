#include "ach/uic/syscalls/Game.hh"

#include "ach/core/op/ProfileInstall.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/Flow.hh"
#include "ach/core/op/AssetsInstall.hh"
#include "ach/core/op/LibrariesInstall.hh"
#include "ach/core/op/NativesCheck.hh"
#include "ach/core/profile/GameProfile.hh"
#include "ach/core/op/AutoProfile.hh"
#include "ach/core/op/Flipper.hh"
#include "ach/core/op/ProfileExt.hh"
#include "ach/uic/UserData.hh"
#include "ach/core/auto/AutoForge.hh"
#include "ach/core/auto/AutoLoader.hh"
#include "ach/core/profile/LaunchProfile.hh"
#include "ach/util/Commons.hh"
#include <cJSON.h>
#include <log.hh>

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void
implGetVersions(ACH_SC_ARGS)
{
  Sys::runOnWorkerThread([&prog, cb]() -> void {
    auto vms = Op::fetchVersionManifest();
    prog.stack.push_back("");
    bool ok = false;
    if(vms.size() > 0)
      {
        cJSON *manifest = cJSON_Parse(vms.c_str());
        if(cJSON_IsObject(manifest))
          {
            cJSON *vers = cJSON_GetObjectItem(manifest, "versions");
            if(cJSON_IsArray(vers))
              {
                ok = true;
                int size = cJSON_GetArraySize(vers);
                for(int i = size - 1; i >= 0; i--)
                  {
                    cJSON *cur = cJSON_GetArrayItem(vers, i);
                    cJSON *id = cJSON_GetObjectItem(cur, "id");
                    cJSON *type = cJSON_GetObjectItem(cur, "type");
                    if(cJSON_IsString(id) && cJSON_IsString(type))
                      {
                        std::string ids = cJSON_GetStringValue(id);
                        std::string types = cJSON_GetStringValue(type);
                        std::string ctype;
                        if(types == "release")
                          {
                            ctype = "R";
                          }
                        else if(types == "snapshot")
                          {
                            ctype = "S";
                          }
                        else
                          {
                            ctype = "O";
                          }
                        prog.stack.push_back(ctype);
                        prog.stack.push_back(
                            ids); // As constant for the translator
                      }
                  }
              }
          }
      }
    prog.carry = ok;
    cb();
  });
}

void
implGetLoaderList(ACH_SC_ARGS)
{
  Sys::runOnWorkerThread([&prog, cb]() -> void {
    auto type = (*UIC::getUserData())["$Loader"];
    auto mcv = (*UIC::getUserData())["$GameVersion"];
    prog.stack.push_back("");
    if(type == "Forge")
      {
        auto vers = Auto::getForgeVersions(mcv);
        vers.reverse();
        if(vers.size() > 0)
          {
            for(auto &v : vers)
              {
                auto urlParts = Commons::splitStr(v, "/");
                auto version = urlParts[6];
                auto fgv = Commons::splitStr(version, "-")[1];
                prog.stack.push_back(fgv);
                prog.stack.push_back(v);
              }
            prog.carry = true;
          }
        else
          {
            prog.carry = false;
          }
      }
    else if(type == "Fabric" || type == "Quilt")
      {
        Auto::LoaderSuite st;
        if(type == "Fabric")
          {
            st = Auto::FABRIC_SUITE;
          }
        else if(type == "Quilt")
          {
            st = Auto::QUILT_SUITE;
          }
        auto vers = Auto::getLoaderVersions(mcv, st);
        vers.reverse();
        if(vers.size() > 0)
          {
            for(auto &v : vers)
              {
                prog.stack.push_back(v);
              }
            prog.carry = true;
          }
        else
          {
            prog.carry = false;
          }
      }
    cb();
  });
}

void
implInstallProfile(ACH_SC_ARGS)
{
  Profile::LaunchProfile lp;
  auto upool = UIC::getUserData();
  auto lpName = (*upool)["$ProfileName"];
  auto accID = (*upool)["$Account"];
  lp.id = Commons::genUUID(lpName);
  lp.displayName = lpName;
  lp.account = accID;
  Op::Flow *flow = new Op::Flow;
  auto mcv = (*upool)["$GameVersion"];
  flow->data[AL_FLOWVAR_PROFILEID] = mcv;
  auto type = (*upool)["$Loader"];
  if(type == "None")
    {
      type = "";
      lp.baseProfile = mcv;
    }
  else
    {
      auto ldv = (*upool)["$LoaderVersion"];
      flow->data[AL_FLOWVAR_LOADERVER] = ldv;
      lp.baseProfile = mcv + "+" + type + "-" + ldv;
    }
  lp.isDemo = false;
  lp.runtime = lp.id; // Use this

  flow->data[AL_FLOWVAR_LOADERTYPE] = type;
  flow->addTask(Op::installProfile);
  flow->addTask(Op::loadProfile);
  flow->addTask(Op::installClient);
  flow->addTask(Op::installLibraries);
  flow->addTask(Op::installAssetIndex);
  flow->addTask(Op::installAssets);
  flow->addTask(Op::copyAssets);
  flow->addTask(Op::collectNatives);
  flow->addTask(Op::autoProfile);
  flow->addTask(Op::flipInstall);
  flow->onProgress = ACH_DEFAULT_PROGRESS;
  flow->onStep = ACH_DEFAULT_STEP;
  flow->run([&prog, cb, flow, lp](bool s) -> void {
    prog.carry = s;
    if(s)
      {
        Profile::LAUNCH_PROFILES.push_back(lp);
      }
    cb();
    delete flow;
  });
}

}
}
}