#include <iostream>
#include <ach/core/op/Flow.hh>
#include <ach/sys/Init.hh>
#include <ach/sys/Schedule.hh>
#include <ach/core/op/ProfileInstall.hh>
#include <ach/core/op/LibrariesInstall.hh>
#include <ach/core/op/AssetsInstall.hh>
#include <unistd.h>
#include <sstream>
#include <fstream>

int
main()
{
  using namespace Alicorn;
  Sys::initSys();
  Op::Flow f;
  f.data[AL_FLOWVAR_PROFILEID] = "1.19.2";
  f.addTask(Op::installProfile);
  f.addTask(Op::installAssetIndex);
  f.addTask(Op::installAssets);
  f.run();
  sleep(114514);
  Sys::downSys();
  return 0;
}