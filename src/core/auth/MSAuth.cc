#include "ach/core/auth/MSAuth.hh"

#include <httplib.h>
#include "ach/util/Proc.hh"
#include "ach/util/Commons.hh"
#include <filesystem>

namespace Alicorn
{
namespace Auth
{

void
msAuth(Profile::AccountProfile &acc)
{
  acc.type = Profile::ACC_MS;
  getMSCode(acc.id, [=](std::string code) -> void {
    if(code.size() == 0)
      {
      }
  });
}

void
getMSCode(const std::string &part, std::function<void(std::string)> cb)
{
  std::string loginExec = "MSLogin!" + part + ".prog";
  if(!std::filesystem::exists(loginExec))
    {
      std::filesystem::copy_file(Commons::argv0, loginExec);
    }
  std::list<std::string> args;
  args.push_back("mslogin");
  Commons::runCommand(loginExec, args, [=](std::string c) -> void {
    std::filesystem::remove(loginExec);

    cb(c);
  });
}

}
}