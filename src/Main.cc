#include "ach/sys/Init.hh"
#include "ach/core/profile/JVMProfile.hh"

int
main()
{
  Alicorn::Sys::initSys();
  if(Alicorn::Profile::JVM_PROFILES.size() == 0)
    {
      Alicorn::Profile::appendJVM("java");
    }
  Alicorn::Sys::downSys();
  return 0;
}