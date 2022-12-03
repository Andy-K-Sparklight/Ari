#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_ARGSGENERATOR
#define INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_ARGSGENERATOR

#include "ach/core/profile/GameProfile.hh"
#include "ach/core/box/Box.hh"
#include <list>
#include <string>

namespace Alicorn
{
namespace Profile
{

class ExternalValues
{
public:
  std::string xuid;
  std::string token;
  std::string uuid;
  std::string playerName;
  bool isDemo = false;
  bool hasCustomRes = false;
  std::string w, h;
  std::list<std::string> extraVMArgs;
  std::string clientID;
};

std::list<std::string> genArgs(const VersionProfile &prof,
                               const Box::GameBox &box,
                               const ExternalValues &ev);

}

}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_ARGSGENERATOR */
