#ifndef INCLUDE_ALICORN_CH_ACH_UIC_SYSCALLS_ACCOUNT
#define INCLUDE_ALICORN_CH_ACH_UIC_SYSCALLS_ACCOUNT

#include "ach/uic/SysCall.hh"

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void implAddLocalAccount(ACH_SC_ARGS);

void implAddMSAccount(ACH_SC_ARGS);

void implValidateYggServer(ACH_SC_ARGS);

void implAddYGAccount(ACH_SC_ARGS);

void implGetAllAccounts(ACH_SC_ARGS);

}
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_SYSCALLS_ACCOUNT */
