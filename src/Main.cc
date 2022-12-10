#include "ach/sys/Init.hh"
#include "ach/core/profile/JVMProfile.hh"
#include "ach/util/Proc.hh"
#include "ach/sys/Schedule.hh"
#include <unistd.h>
#include <list>
#include <string>

int
main()
{
  Alicorn::Sys::initSys();

  Alicorn::Profile::appendJVM(
      "java", [](bool s) -> void { printf("ADD SUCC?: %d\n", s); });

  sleep(3);
  Alicorn::Sys::downSys();
  return 0;
}