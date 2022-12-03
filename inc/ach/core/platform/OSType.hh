#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_OSTYPE
#define INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_OSTYPE

namespace Alicorn
{

namespace Platform
{

enum OSType
{
  OS_MSDOS,
  OS_DARWIN,
  OS_UNIX,
};

extern OSType OS_TYPE;

enum OSArch
{
  AR_X86,
  AR_OTHER,
};

extern OSArch OS_ARCH;

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_OSTYPE */
