#ifndef INCLUDE_ALICORN_CH_ACH_UIC_SYSCALL
#define INCLUDE_ALICORN_CH_ACH_UIC_SYSCALL

#include "ach/uic/Brain.hh"

#define ACH_SC_ARGS Program &prog, PCallback cb

namespace Alicorn
{
namespace UIC
{

void bindAllSysCalls();

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_SYSCALL */
