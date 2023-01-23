#include "ach/uic/syscalls/Account.hh"

#include "ach/core/auth/LocalAuth.hh"
#include "ach/core/auth/MSAuth.hh"
#include "ach/core/auth/YggAuth.hh"
#include "ach/core/profile/AccountProfile.hh"
#include "ach/sys/Schedule.hh"
#include <lurl.hpp>
#include <set>

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
    prog.carry = s;
    if(s)
      {
        prog.stack.push_back(ac->userName);
        Profile::ACCOUNT_PROFILES.push_back(*ac);
        delete ac;
      }
    cb();
  });
}

// We'll check other sites and validate them
#define ACH_TRUST_YGG_HOSTS                                                   \
  {                                                                           \
    "https://littleskin.cn", "https://account.ely.by", "https://mcskin.cn"    \
  }

void
implValidateYggServer(ACH_SC_ARGS)
{
  std::string su = prog.locals["%ServerAddr"];
  auto u = LUrlParser::ParseURL::parseURL(su);
  std::set<std::string> trustHosts = ACH_TRUST_YGG_HOSTS;
  prog.carry = trustHosts.contains(u.connectionName_);
  cb();
}

void
implAddYGAccount(ACH_SC_ARGS)
{
  std::string server = prog.locals["%ServerAddr"];
  std::string user = prog.locals["%UserName"];
  std::string pwd = prog.locals["%Pwd"];

  Sys::runOnWorkerThread([=, &prog]() -> void {
    auto acc = Auth::mkYggAccount(user, server);

    // That is a sync method
    if(Auth::yggAuth(acc, pwd))
      {
        prog.carry = true;
        prog.stack.push_back(acc.userName);
        Profile::ACCOUNT_PROFILES.push_back(acc);
      }
    else
      {
        prog.carry = false;
      }
    cb();
  });
}

void
implGetAllAccounts(ACH_SC_ARGS)
{
  prog.stack.push_back("");
  for(auto &a : Profile::ACCOUNT_PROFILES)
    {
      prog.stack.push_back(std::to_string(a.type));
      prog.stack.push_back(a.userName);
      prog.stack.push_back(a.email);
      prog.stack.push_back(a.id);
    }
  cb();
}

}
}
}