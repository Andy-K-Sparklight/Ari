#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_TRANSFORM
#define INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_TRANSFORM

#include <string>
#include <cJSON.h>

namespace Alicorn
{
namespace Profile
{

// Generates a new profile transformed from the source.
// The caller is responsible for freeing the memory.
cJSON *transformProfile(const cJSON *src);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_TRANSFORM */
