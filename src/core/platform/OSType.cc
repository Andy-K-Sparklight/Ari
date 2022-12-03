#include "ach/core/platform/OSType.hh"

namespace Alicorn
{
namespace Platform
{

#if defined(_WIN32)
OSType OS_TYPE = OS_MSDOS;
#elif defined(__APPLE__)
OSType OS_TYPE = OS_DARWIN;
#else
OSType OS_TYPE = OS_UNIX;
#endif

#if defined(__i386__) || defined(__x86_64__)
OSArch OS_ARCH = AR_X86;
#else
OSArch OS_ARCH = AR_OTHER;
#endif
}
}