#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_GAMEPROFILE
#define INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_GAMEPROFILE

#include <string>
#include <list>
#include <cJSON.h>

namespace Alicorn
{

namespace Profile
{

// The criteria type
enum RuleCriteriaType
{
  CR_OS_NAME,
  CR_OS_VERSION,
  CR_OS_ARCH, // Same as above
  CR_FE_DEMO, // Demo
  CR_FE_CRES, // Custom resolution
  CR_ALWAYS,  // Always true
};

// The type of the version profile
enum ReleaseType
{
  RT_RELEASE,
  RT_SNAPSHOT,
  RT_ALPHA,
  RT_BETA
};

// Judge rule
class Rule
{
public:
  bool action;
  RuleCriteriaType criteria;
  std::string condition;

  Rule(const cJSON *src);
};

// Downloadable artifact unit
class Artifact
{
public:
  std::string path, sha1, url;
  size_t size = 0;

  Artifact(const cJSON *src);

  // DO NOT use this its just a fallback
  Artifact();
};

// One library
// Note that normal and native ones look the same
class Library
{
public:
  std::string name;
  Artifact artifact;
  std::list<Rule> rules;
  bool isNative;

  Library(const cJSON *src);
};

class Argument
{
public:
  std::list<std::string> values;
  std::list<Rule> rules;

  Argument(const cJSON *src);
};

class VersionProfile
{
public:
  std::string id;
  std::list<Argument> gameArgs;
  std::list<Argument> jvmArgs;
  Artifact assetIndexArtifact;
  Artifact clientArtifact;
  Artifact clientMappingsArtifact; // Forge
  unsigned int javaVersion = 0;    // If specified
  std::list<Library> libraries;
  std::string logArgument;
  Artifact logConfigFileArtifact;
  std::string mainClass;
  std::string releaseTime;
  std::string installTime;
  ReleaseType type = RT_RELEASE;

  VersionProfile(const cJSON *src);
};

typedef struct
{
  std::string hash;
  size_t size;
} Asset;

std::list<Asset> loadAssetIndex(const std::string &src);

}

}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PROFILE_GAMEPROFILE */
