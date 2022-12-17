#include "ach/sys/Init.hh"
#include "ach/core/profile/JVMProfile.hh"
#include "ach/util/Proc.hh"
#include "ach/core/op/Flow.hh"
#include "ach/sys/Schedule.hh"
#include "ach/core/op/AssetsInstall.hh"
#include "ach/core/op/Flipper.hh"
#include "ach/core/op/ProfileInstall.hh"
#include "ach/core/auth/LocalAuth.hh"
#include "ach/core/op/LibrariesInstall.hh"
#include "ach/core/op/ProfileExt.hh"
#include "ach/core/op/GameLaunch.hh"
#include "ach/core/op/Authenticate.hh"
#include "ach/sys/Storage.hh"
#include "ach/util/Commons.hh"
#include "ach/core/op/NativesCheck.hh"

#include <unistd.h>
#include <list>
#include <cJSON.h>
#include <string>
#include <iostream>

int
main()
{
  Alicorn::Sys::initSys();
  Alicorn::Op::Flow flow;
  flow.data[AL_FLOWVAR_PROFILEID] = "1.19.2";
  flow.data[AL_FLOWVAR_RUNTIME] = "test";
  auto a = Alicorn::Auth::mkLocalAccount("skjsjhb");
  flow.data[AL_FLOWVAR_ACCOUNTINDEX] = "0";
  Alicorn::Profile::ACCOUNT_PROFILES.push_back(a);
  flow.data[AL_FLOWVAR_DEMO] = "0";
  flow.data[AL_FLOWVAR_WIDTH] = "1920";
  flow.data[AL_FLOWVAR_HEIGHT] = "1080";
  Alicorn::Sys::setValue(ACH_CFG_DLM, ACH_CFG_DLM_ACC);
  flow.data[AL_FLOWVAR_JAVAMAIN] = "java";
  flow.addTask(Alicorn::Op::installProfile);
  flow.addTask(Alicorn::Op::flipInstall);
  flow.addTask(Alicorn::Op::loadProfile);
  flow.addTask(Alicorn::Op::installClient);
  flow.addTask(Alicorn::Op::installLibraries);
  flow.addTask(Alicorn::Op::flipInstall);
  flow.addTask(Alicorn::Op::collectNatives);
  flow.addTask(Alicorn::Op::installAssetIndex);
  flow.addTask(Alicorn::Op::installAssets);
  flow.addTask(Alicorn::Op::flipInstall);
  flow.addTask(Alicorn::Op::authAccount);
  flow.addTask(Alicorn::Op::launchGame);
  flow.run();
  sleep(3000);
  Alicorn::Sys::downSys();
  return 0;
}