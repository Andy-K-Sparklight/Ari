#include "ach/core/op/AutoProfile.hh"

#include "ach/core/auto/AutoLoader.hh"

namespace Alicorn
{
namespace Op
{
void
autoProfile(Flow *flow, FlowCallback cb)
{
  std::string variant = flow->data[AL_FLOWVAR_LOADERTYPE];
  auto mcv = flow->data[AL_FLOWVAR_PROFILEID];
  auto ldv = flow->data[AL_FLOWVAR_LOADERVER];
  if(variant == "Fabric")
    {
      if(!Auto::installLoader(mcv, ldv, Auto::FABRIC_SUITE))
        {
          cb(AL_ERR);
          return;
        }
    }
  else if(variant == "Quilt")
    {
      if(!Auto::installLoader(mcv, ldv, Auto::QUILT_SUITE))
        {
          cb(AL_ERR);
          return;
        }
    }
  cb(AL_OK);
}
}
}