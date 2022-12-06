#include <iostream>
#include <ach/core/op/Flow.hh>
#include <ach/sys/Init.hh>
#include <ach/sys/Schedule.hh>
#include <ach/core/op/ProfileInstall.hh>
#include <ach/core/op/LibrariesInstall.hh>
#include <ach/core/op/AssetsInstall.hh>
#include <ach/core/op/NativesCheck.hh>
#include <ach/core/op/GameLaunch.hh>
#include <ach/core/op/Tools.hh>
#include <ach/core/op/Flipper.hh>
#include <unistd.h>
#include <sstream>
#include <fstream>

int
main()
{
  using namespace Alicorn;
  Sys::initSys();

  Op::Flow f;
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
  f.data[AL_FLOWVAR_PROFILEID] = "1.19.2";
  f.data[AL_FLOWVAR_LAUNCHVALS] = s;
  f.data[AL_FLOWVAR_JAVAMAIN] = "java";
  f.addTask(Op::installProfile);
  // f.addTask(Op::installClient);
  // f.addTask(Op::installLibraries);
  // f.addTask(Op::flipInstall);
  // f.addTask(Op::checkNatives);
  // f.addTask(Op::installAssetIndex);
  // f.addTask(Op::installAssets);
  //  f.addTask(Op::flipInstall);

  f.addTask(Op::launchGame);
  f.run();
  sleep(114514);
  Sys::downSys();
  return 0;
}