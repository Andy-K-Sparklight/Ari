#include "ach/core/op/AutoProfile.hh"

#include "ach/core/auto/AutoLoader.hh"
#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Op
{
void
autoProfile(Flow *flow, FlowCallback cb)
{
  // In the format of 'Type1,Type2,Type3'
  auto variants = Commons::splitStr(flow->data[AL_FLOWVAR_LOADERTYPE], ",");
  auto mcv = flow->data[AL_FLOWVAR_PROFILEID];
  auto ldvs = Commons::splitStr(flow->data[AL_FLOWVAR_LOADERVER], ",");
  if(ldvs.size() != variants.size())
    {
      cb(AL_ERR);
      return;
    }
  for(size_t i = 0; i < variants.size(); i++)
    {
      auto &variant = variants[i];
      auto &ldv = ldvs[i];
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
    }

  cb(AL_OK);
}
}
}