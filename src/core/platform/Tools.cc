#include "ach/core/platform/Tools.hh"

#include <uv.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "ach/sys/Schedule.hh"
#include <zip.h>
#include "ach/util/Commons.hh"

namespace Alicorn
{
namespace Platform
{

static void
openFileAsync(const std::string &pt, int flag, int mode,
              std::function<void(int)> callback)
{
  uv_fs_t *openReq = new uv_fs_t;
  auto func = new std::function<void(int)>;
  *func = callback;
  openReq->data = func;
  uv_fs_open(uv_default_loop(), openReq, pt.c_str(), flag, mode,
             [](uv_fs_t *req) -> void {
               auto f = (std::function<void(int)> *)req->data;
               (*f)(req->result);
               delete f;
               delete req;
             });
}

static void
openDirAsync(const std::string &pt, std::function<void(uv_dir_t *)> callback)
{
  uv_fs_t *openReq = new uv_fs_t;
  auto func = new std::function<void(uv_dir_t *)>;
  *func = callback;
  openReq->data = func;
  uv_fs_opendir(uv_default_loop(), openReq, pt.c_str(),
                [](uv_fs_t *req) -> void {
                  auto f = (std::function<void(uv_dir_t *)> *)req->data;
                  (*f)((uv_dir_t *)req->ptr);
                  delete f;
                  delete req;
                });
}

static void
closeFileAsync(int fd, std::function<void()> callback)
{
  uv_fs_t *closeReq = new uv_fs_t;
  auto func = new std::function<void()>;
  *func = callback;
  closeReq->data = func;
  uv_fs_close(uv_default_loop(), closeReq, fd, [](uv_fs_t *req) -> void {
    auto f = (std::function<void()> *)req->data;
    (*f)();
    delete f;
    delete req;
  });
}

static void
closeDirAsync(uv_dir_t *fd, std::function<void()> callback)
{
  uv_fs_t *closeReq = new uv_fs_t;
  auto func = new std::function<void()>;
  *func = callback;
  closeReq->data = func;
  uv_fs_closedir(uv_default_loop(), closeReq, fd, [](uv_fs_t *req) -> void {
    auto f = (std::function<void()> *)req->data;
    (*f)();
    delete f;
    delete req;
  });
}

typedef struct
{
  uv_buf_t bufReq;
  char *buf;
  std::function<void(bool)> callback;
  int fd;
} WriteFileReqCarry;

void
writeFileAsync(const std::string &pt, const char *data, size_t dataLength,
               std::function<void(bool)> callback)
{
  openFileAsync(pt, O_WRONLY | O_CREAT, 0775, [=](int fd) -> void {
    if(fd < 0)
      {
        callback(false);
      }
    else
      {
        uv_fs_t *writeReq = new uv_fs_t;
        char *buf = new char[dataLength];
        memcpy(buf, data, dataLength);
        WriteFileReqCarry *carry = new WriteFileReqCarry;
        carry->buf = buf;
        carry->bufReq = uv_buf_init(buf, dataLength * sizeof(char));
        carry->callback = callback;
        carry->fd = fd;
        writeReq->data = carry;
        uv_fs_write(uv_default_loop(), writeReq, fd, &carry->bufReq, 1, 0,
                    [](uv_fs_t *req) -> void {
                      auto carry = (WriteFileReqCarry *)req->data;
                      delete[] carry->buf;
                      closeFileAsync(carry->fd, [=]() -> void {
                        carry->callback(req->result < 0);
                        delete carry;
                        delete req;
                      });
                    });
      }
  });
}

void
copyFileAsync(const std::string &src, const std::string &dest,
              std::function<void(bool)> callback)
{
  uv_fs_t *copyReq = new uv_fs_t;
  auto *funcCpy = new std::function<void(bool)>;
  *funcCpy = callback;
  copyReq->data = funcCpy;
  uv_fs_copyfile(uv_default_loop(), copyReq, src.c_str(), dest.c_str(),
                 UV_FS_COPYFILE_FICLONE, [](uv_fs_t *req) -> void {
                   auto *cb = (std::function<void(bool)> *)req->data;
                   if(req->result < 0)
                     {
                       (*cb)(false);
                     }
                   else
                     {
                       (*cb)(true);
                     }
                   delete cb;
                   delete req;
                 });
}

void
mkParentDirs(const std::string &pt)
{
  // We choose to use the std function.
  // It might be a bit slower, but are less likely to cause bugs than
  // implementing the uv version on our own
  auto pat = std::filesystem::path(
      Commons::normalizePath(std::filesystem::path(pt)));
  std::filesystem::create_directories(pat.parent_path());
}

void
mkParentDirs(const std::filesystem::path &pt)
{
  auto pat = std::filesystem::path(Commons::normalizePath(pt));
  std::filesystem::create_directories(pat.parent_path());
}

typedef struct
{
  std::function<void(std::list<std::string>)> callback;
  uv_dir_t *fd;

} ReadDirectoryCarry;

void
readDirAsync(const std::string &pt,
             std::function<void(std::list<std::string>)> callback)
{
  uv_fs_t *readReq = new uv_fs_t;
  auto carry = new ReadDirectoryCarry;
  carry->callback = callback;
  readReq->data = carry;
  openDirAsync(pt, [=](uv_dir_t *fd) -> void {
    if(fd == NULL)
      {
        std::list<std::string> l;
        callback(l);
        delete readReq;
        delete carry;
        return;
      }
    carry->fd = fd;
    fd->dirents = new uv_dirent_t[512];
    fd->nentries = 512;
    uv_fs_readdir(uv_default_loop(), readReq, fd, [](uv_fs_t *req) -> void {
      auto carry = (ReadDirectoryCarry *)req->data;
      uv_dir_t *tfd = carry->fd;
      if(req->result < 0)
        {
          uv_fs_req_cleanup(req);
          closeDirAsync(tfd, [=]() -> void {
            std::list<std::string> emptyList;
            carry->callback(emptyList);
            delete carry;
            delete req;
          });
        }
      else
        {
          std::list<std::string> outList;
          std::string suffix;
          ssize_t ent = req->result;
          for(ssize_t i = 0; i < ent; i++)
            {
              auto cur = carry->fd->dirents[i];
              if(cur.type == UV__DT_FILE)
                {
                  suffix = "F";
                }
              else if(cur.type == UV__DT_DIR)
                {
                  suffix = "D";
                }
              else
                {
                  suffix = "U";
                }
              outList.push_back(cur.name + suffix);
            }
          uv_fs_req_cleanup(req);

          closeDirAsync(tfd, [=]() -> void {
            carry->callback(outList);
            delete carry;
            delete req;
          });
        }
    });
  });
}

// Modified from https://github.com/richgel999/miniz/issues/38
bool
unzipFile(const std::string &zipFile, const std::string &pt)
{
  return zip_extract(zipFile.c_str(), pt.c_str(), NULL, NULL) >= 0;
}

bool
zipDir(const std::string &prefix, const std::string &fileName)
{
  auto files = scanDirectory(prefix);
  auto zip = zip_open(fileName.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  for(auto &f : files)
    {
      auto relPt = std::filesystem::relative(f, prefix);
      zip_entry_open(zip, relPt.string().c_str());
      if(zip_entry_fwrite(zip, f.c_str()) < 0)
        {
          return false; // Broken
        }
      zip_entry_close(zip);
    }
  zip_close(zip);
  return true;
}

std::list<std::string>
scanDirectory(const std::filesystem::path pt, bool includeDir)
{

  std::list<std::string> out;
  try
    {
      for(std::filesystem::recursive_directory_iterator i(pt), end; i != end;
          ++i)
        {
          if(i->is_regular_file())
            {
              out.push_back(i->path().string());
            }
          else if(includeDir && i->is_directory())
            {
              out.push_back(i->path().string() + "/");
            }
        };
    }
  catch(std::exception &ignored)
    {
    }
  return out;
}

}

}