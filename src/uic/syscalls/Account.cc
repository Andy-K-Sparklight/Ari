#include "ach/uic/syscalls/Account.hh"

#include "ach/core/auth/LocalAuth.hh"
#include "ach/core/auth/MSAuth.hh"
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
  cb();
}

void
implAddMSAccount(ACH_SC_ARGS)
{
  auto ac = new Profile::AccountProfile();
  *ac = Auth::mkMsAccount();
  Auth::msAuth(*ac, [&prog, ac, cb](bool s) -> void {
    prog.carry = !s;
    if(s)
      {
        prog.stack.push_back(ac->userName);
        Profile::ACCOUNT_PROFILES.push_back(*ac);
        delete ac;
      }
    cb();
  });
}

}
}
}