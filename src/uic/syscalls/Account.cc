#include "ach/uic/syscalls/Account.hh"

#include "ach/core/auth/LocalAuth.hh"
#include "ach/core/profile/AccountProfile.hh"

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void
implAddLocalAccount(ACH_SC_ARGS)
{
  auto pName = prog.locals["%UserName"];
  auto ac = Auth::mkLocalAccount(pName);
  Profile::ACCOUNT_PROFILES.push_back(ac);
}

}
}
}