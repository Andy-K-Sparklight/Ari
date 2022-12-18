#include "ach/core/op/ProfileExt.hh"

#include "ach/core/profile/GameProfile.hh"
#include "ach/core/op/Tools.hh"
#include "ach/core/op/Finder.hh"
#include "ach/sys/Schedule.hh"

#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <log.hh>

namespace Alicorn
{
namespace Op
{

static void
mergeProfile(Profile::VersionProfile &mod, const Profile::VersionProfile &base)
{
  // Fix libraries
  for(auto &l : base.libraries)
    {
      mod.libraries.push_back(l);
    }

  // Assign assets
  if(mod.assetIndexArtifact.path.size() == 0)
    {
      mod.assetIndexArtifact = base.assetIndexArtifact;
    }

  // Assign args
  for(auto &a : base.gameArgs)
    {
      mod.gameArgs.push_back(a);
    }
  for(auto &a : base.jvmArgs)
    {
      mod.jvmArgs.push_back(a);
    }

  mod.inheritsFrom = base.inheritsFrom; // Pass it on

  // Copy others
  mod.clientMappingsArtifact = base.clientMappingsArtifact;
  mod.javaVersion = base.javaVersion;
  mod.logArgument = base.logArgument;
  mod.logConfigFileArtifact = base.logConfigFileArtifact;
}

static Profile::VersionProfile
readProfile(const std::string &id, Flow *flow)
{
  // Performance is quite acceptable
  std::ifstream f(
      getStoragePath(std::string("versions/") + id + "/" + id + ".json"));
  if(f.fail())
    {
      return Profile::VersionProfile();
    }
  std::stringstream ss;
  ss << f.rdbuf();
  std::string s = ss.str();
  Profile::VersionProfile p(s);
  // So that it will be overwritten by the last call
  flow->data[AL_FLOWVAR_INHCLIENTSRC] = "versions/" + p.clientArtifact.path;
  if(p.inheritsFrom.size() > 0)
    {
      auto inhProf = readProfile(p.inheritsFrom, flow);
      if(inhProf.id.size() > 0)
        {
          mergeProfile(p, inhProf); // Assign
          LOG("Merged profile " << p.id << " <- " << inhProf.id);
        }
    }
  return p;
}

void
loadProfile(Flow *flow, FlowCallback cb)
{
  cb(AL_RDPROFILE);
  auto id = flow->data[AL_FLOWVAR_PROFILEID];
  if(id.size() == 0)
    {
      cb(AL_ERR);
      return;
    }
  LOG("Loading profile " << id);
  Sys::runOnWorkerThread([=]() -> void {
    auto p = readProfile(id, flow);
    LOG("Loaded profile " << id);
    flow->profile = p;
    cb(AL_OK);
  });
}

void
linkClient(Flow *flow, FlowCallback cb)
{
  cb(AL_LINKCLIENT);
  auto source = getStoragePath(flow->data[AL_FLOWVAR_INHCLIENTSRC]);
  auto id = flow->data[AL_FLOWVAR_PROFILEID];
  if(source.size() == 0 || id.size() == 0)
    {
      cb(AL_OK);
      return;
    }
  LOG("Linking client for " << id);
  Sys::runOnWorkerThread([=]() -> void {
    std::string target = getInstallPath("versions/" + id + "/" + id + ".jar");
    if(!std::filesystem::exists(target))
      {
        try
          {
            mkParentDirs(target);
            std::filesystem::copy_file(source, target);
          }
        catch(std::exception &e)
          {
            LOG("Could not link client for " << id);
            cb(AL_ERR);
            return;
          }
      }
    LOG("Linked client " << target << " <- " << source);
    cb(AL_OK);
  });
}

}
}