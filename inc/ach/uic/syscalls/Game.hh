#ifndef INCLUDE_ALICORN_CH_ACH_UIC_SYSCALLS_GAME
#define INCLUDE_ALICORN_CH_ACH_UIC_SYSCALLS_GAME

#include "ach/uic/SysCall.hh"

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void implGetVersions(ACH_SC_ARGS);

void implGetLoaderList(ACH_SC_ARGS);

void implInstallProfile(ACH_SC_ARGS);

void implLaunchGame(ACH_SC_ARGS);

void implMonitorGame(ACH_SC_ARGS);

void implGetLaunchProfiles(ACH_SC_ARGS);

}
}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_SYSCALLS_GAME */
