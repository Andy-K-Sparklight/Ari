#ifndef INCLUDE_ALICORN_CH_ACH_UIC_USERDATA
#define INCLUDE_ALICORN_CH_ACH_UIC_USERDATA

#include "ach/uic/Brain.hh"

namespace Alicorn
{
namespace UIC
{

DataPool *getUserData();

void loadUserData();
void saveUserData();

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_USERDATA */
