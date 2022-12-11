#include "ach/sys/Init.hh"
#include "ach/core/profile/JVMProfile.hh"
#include "ach/util/Proc.hh"
#include "ach/core/op/Flow.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/AssetsInstall.hh"
#include "ach/core/op/Flipper.hh"
#include "ach/core/op/ProfileInstall.hh"
#include "ach/core/op/LibrariesInstall.hh"
#include "ach/core/op/GameLaunch.hh"
#include "ach/core/op/NativesCheck.hh"

#include <unistd.h>
#include <list>
#include <cJSON.h>
#include <string>

int
main()
{
  Alicorn::Sys::initSys();
  Alicorn::Op::Flow flow;
  flow.data[AL_FLOWVAR_PROFILEID] = "1.12.2";
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "xuid", "aaa");
  cJSON_AddStringToObject(json, "uuid", "aaa");
  cJSON_AddStringToObject(json, "token", "aaa");
  cJSON_AddStringToObject(json, "playerName", "Dev");
  cJSON_AddStringToObject(json, "w", "1920");
  cJSON_AddStringToObject(json, "h", "1080");
  cJSON_AddStringToObject(json, "runtimeName", "test");
  cJSON_AddBoolToObject(json, "isDemo", false);
  cJSON_AddBoolToObject(json, "hasCustomRes", true);
  std::string s = cJSON_Print(json);
  cJSON_Delete(json);

  flow.data[AL_FLOWVAR_LAUNCHVALS] = s;
  flow.data[AL_FLOWVAR_JAVAMAIN] = "java";
  flow.addTask(Alicorn::Op::installProfile);
  flow.addTask(Alicorn::Op::installClient);
  flow.addTask(Alicorn::Op::installLibraries);
  flow.addTask(Alicorn::Op::flipInstall);
  flow.addTask(Alicorn::Op::collectNatives);
  flow.addTask(Alicorn::Op::installAssetIndex);
  flow.addTask(Alicorn::Op::installAssets);
  flow.addTask(Alicorn::Op::flipInstall);
  flow.addTask(Alicorn::Op::launchGame);
  flow.run();
  sleep(3000);
  Alicorn::Sys::downSys();
  return 0;
}