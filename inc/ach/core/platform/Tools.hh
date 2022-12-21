#ifndef INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_TOOLS
#define INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_TOOLS

#include <functional>
#include <string>
#include <list>
#include <filesystem>

namespace Alicorn
{
namespace Platform
{
// All functions listed here should run on the correct thread!

// Write to a file on UV thread
void writeFileAsync(const std::string &pt, const char *data, size_t dataLength,
                    std::function<void(bool)> callback);

// Copy a file on UV thread
void moveFileAsync(const std::string &src, const std::string &dest,
                   std::function<void(bool)> callback);

// Create parent directories on Worker thread
void mkParentDirs(const std::string &pt);

void mkParentDirs(const std::filesystem::path &pt);

// Read directory
void readDirAsync(const std::string &pt,
                  std::function<void(std::list<std::string>)> callback);

// Unzip file
bool unzipFile(const std::string &f, const std::string &extractPrefix);

// Zip dir
bool zipDir(const std::string &prefix, const std::string &fileName);

// Scan directory
std::list<std::string> scanDirectory(const std::filesystem::path pt,
                                     bool includeDir = false);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_PLATFORM_TOOLS */
