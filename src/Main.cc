#include <iostream>
#include <ach/core/op/Flow.hh>
#include <ach/sys/Init.hh>
#include <ach/sys/Schedule.hh>
#include <ach/core/op/ProfileInstall.hh>
#include <ach/core/op/LibrariesInstall.hh>
#include <unistd.h>

int
main()
{
  using namespace Alicorn;
  Sys::initSys();
  Op::Flow flow;
  flow.data[AL_FLOWVAR_PROFILEID] = "1.19.2";
  flow.addTask(Op::installProfile);
  flow.addTask(Op::installLibraries);
  flow.run();
  sleep(114514);
  Sys::downSys();
  return 0;
}