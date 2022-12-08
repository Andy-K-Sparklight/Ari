#ifndef INCLUDE_ALICORN_CH_ACH_CORE_NETWORK_DOWNLOAD
#define INCLUDE_ALICORN_CH_ACH_CORE_NETWORK_DOWNLOAD

#include <list>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <functional>
#include "ach/core/profile/GameProfile.hh"

namespace Alicorn
{

namespace Network
{

// Minimum required data to perform download
class DownloadMeta
{
public:
  std::string baseURL; // Mirrors will apply
  std::string path;    // Target path
  std::string hash;    // Hash, in the format <TYPE>=<HEX>
  size_t size = 0;     // Total size

  // DO NOT use this for library!
  static DownloadMeta mkFromArtifact(const Profile::Artifact &artifact,
                                     const std::string &pathPrefix);
  // For library only
  static DownloadMeta mkFromLibrary(const Profile::Library &lib,
                                    const std::string &pathPrefix);
};

// Just like a process, but in the download env
class DownloadProcess
{
  // There are no member functions, as we normally use batch operaions
public:
  std::list<std::string> urls;                 // Mirrors included
  std::string path;                            // Target path
  std::string gid;                             // Aria2 GID
  std::string hash;                            // Hash
  size_t completedLength = 0, totalLength = 0; // Total size
  size_t speed = 0;                            // Current speed
  std::string stat;                            // Status
};

typedef struct
{
  size_t completedSize = 0, totalSize = 0, failedSize = 0, speed = 0;
  unsigned int completed = 0, failed = 0, total = 0;
} DownloadPackStatus;

// A manager to handle some processes
// It will act as a controller
class DownloadPack
{
protected:
  std::set<std::string> existingPaths;
  std::vector<DownloadProcess> procs;
  std::function<void(const DownloadPack *pack)> updateFunc;

public:
  // Create process using specified meta. Returns the index.
  unsigned int addTask(const DownloadMeta &meta);

  // Commit data, which also starts the download process
  // Note that this is blocking, create new thread if possible
  void commit();

  // Sync status, should be called frequently
  // This is also blocking
  void sync();

  // Get status
  DownloadPackStatus getStat() const;

  // Get proc detail
  DownloadProcess getTask(unsigned int id) const;

  // Called when self status update
  void assignUpdateCallback(std::function<void(const DownloadPack *proc)> cb);
};

extern std::vector<DownloadPack> ALL_DOWNLOADS;

// Start to sync all packs frequently
void setupDownloadsSync();

// Stop sync
void stopDownloadsSync();

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_NETWORK_DOWNLOAD */
