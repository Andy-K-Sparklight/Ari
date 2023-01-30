#include "ach/util/Commons.hh"

#include <string>
#include <random>
#include <uuid.h>
#include <sys/stat.h>
#include <openssl/md5.h>

namespace Alicorn
{
namespace Commons
{

std::string argv0;

std::vector<std::string>
splitStr(const std::string &s, const std::string &delimiter, int max)
{

  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;
  if(s.size() == 0 || !s.contains(delimiter))
    {
      res.push_back(s);
      return res;
    }
  int i = 0;
  while((pos_end = s.find(delimiter, pos_start)) != std::string::npos
        && i != max)
    {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
      i++;
    }

  res.push_back(s.substr(pos_start));
  return res;
}

std::string
normalizePath(const std::filesystem::path &pt)
{
  std::filesystem::path path(pt);
  std::filesystem::path canonicalPath
      = std::filesystem::weakly_canonical(path);
  std::string npath = canonicalPath.make_preferred().string();
  return npath;
}

std::string
getNameHash(const std::string &name)
{
  std::hash<std::string> stringHasher;
  auto sz = stringHasher(name);
  return std::to_string(sz);
}

std::string
genUUID()
{
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{ generator };
  auto id = gen();
  return uuids::to_string(id);
}

std::string
genUUID(const std::string &name)
{
  unsigned char result[MD5_DIGEST_LENGTH];
  MD5((unsigned char *)name.c_str(), name.size(), result);
  uuids::uuid id{ result };
  return uuids::to_string(id);
}

ssize_t
getFileSize(const std::string &path)
{
  struct stat stat_buf;
  int rc = stat(path.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

}
}