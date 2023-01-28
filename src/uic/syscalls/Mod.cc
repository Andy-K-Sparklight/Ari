#include "ach/uic/syscalls/Mod.hh"

#include "ach/extra/mod/provider/Modrinth.hh"

namespace Alicorn
{
namespace UIC
{
namespace SysCall
{

void
implOpenModrinth(ACH_SC_ARGS)
{
  Extra::Mod::Modrinth::showModrinthWindow([cb]() -> void { cb(); });
}

}
}
}