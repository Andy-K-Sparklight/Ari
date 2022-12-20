#include "ach/extra/AutoJVM.hh"

#include "ach/util/Proc.hh"
#include "ach/util/Commons.hh"
#include "ach/core/platform/OSType.hh"

#include <log.hh>

namespace Alicorn
{
namespace Extra
{

void
scanJVM(std::function<void(std::list<std::string>)> cb)
{
  LOG("Scanning for JVMs.");
  std::list<std::string> args;
  std::string bin;
  if(Platform::OS_TYPE == Platform::OS_MSDOS)
    {
      bin = "where";
    }
  else
    {
      bin = "which";
      args.push_back("-a");
    }
  args.push_back("java");
  Commons::runCommand(
      bin, args,
      [=](std::string output, int) -> void {
        std::list<std::string> jvms;
        auto dat = Commons::splitStr(output, "\n");
        for(auto &b : dat)
          {
            if(b.ends_with("\r"))
              {
                b.pop_back();
              }
            if(b.size() > 0)
              {
                jvms.push_back(b);
              }
          };
        LOG(jvms.size() << " JVMs found.");
        cb(jvms);
      },
      1);
}

void
installJVM(std::function<void(bool)> cb)
{
}
}
}