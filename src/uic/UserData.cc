#include "ach/uic/UserData.hh"
#include "ach/sys/Storage.hh"

namespace Alicorn
{
namespace UIC
{

static DataPool GLOBAL_DATA_CTL;

DataPool *
getUserData()
{
  return &GLOBAL_DATA_CTL;
}

void
loadUserData()
{
  auto v = Sys::loadKVG("UserData.kv");
  if(v.size() > 0)
    {
      GLOBAL_DATA_CTL = v[0];
    }
}

void
saveUserData()
{
  DataPool mapDump;
  for(auto &p : GLOBAL_DATA_CTL)
    {
      if(p.first.starts_with("$$"))
        {
          mapDump[p.first] = p.second;
        }
    }
  Sys::saveKVG("UserData.kv", { mapDump });
}

}
}