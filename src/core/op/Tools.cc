#include "ach/core/op/Tools.hh"

#include <uv.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "ach/sys/Schedule.hh"
#include <minizip/unzip.h>

namespace Alicorn
{
namespace Op
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
mkParentDirs(const std::string &pt)
{
  // We choose to use the std function.
  // It might be a bit slower, but are less likely to cause bugs than
  // implementing the uv version on our own
  auto pat = std::filesystem::absolute(std::filesystem::path(pt));
  std::filesystem::create_directories(pat.parent_path());
}

void
mkParentDirs(const std::filesystem::path &pt)
{
  // We choose to use the std function.
  // It might be a bit slower, but are less likely to cause bugs than
  // implementing the uv version on our own
  auto pat = std::filesystem::absolute(pt);
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

bool
unzipFile(const std::string &f, const std::string &extractPrefix)
{
  unzFile file = unzOpen(f.c_str());
  if(file == NULL)
    {
      return false;
    }
  unz_global_info fileInfo;
  int e;
  if((e = unzGetGlobalInfo(file, &fileInfo)) != UNZ_OK)
    {
      return false;
    }
  for(uint32_t i = 0; i < fileInfo.number_entry; i++)
    {
      char fName[256];
      unz_file_info currentFileInfo;
      if((e = unzGetCurrentFileInfo(file, &currentFileInfo, fName,
                                    sizeof(fName), NULL, 0, NULL, 0))
         != UNZ_OK)
        {
          unzGoToNextFile(file);
          continue; // Just do the next one
        };
      std::string strFName = fName;
      if(strFName.ends_with("/")) // Directory
        {
          unzGoToNextFile(file);
          continue;
        }
      auto actualPt = std::filesystem::path(extractPrefix) / strFName;
      mkParentDirs(actualPt);
      char buf[8192];
      e = unzOpenCurrentFile(file);

      if(e != UNZ_OK)
        {
          unzGoToNextFile(file);
          continue;
        }
      // A sync method will be OK, not too much performance impact
      do
        {
          std::ofstream outFile(actualPt);
          e = unzReadCurrentFile(file, buf, sizeof(buf));
          if(e > 0)
            {
              outFile.write(buf, e); // The e is size
            }
          else if(e < 0)
            {
              outFile.close();
              std::filesystem::remove(actualPt); // Problem reading
              break;
            }
          else
            {
              outFile.close(); // Finished
              break;
            }
        }
      while(e > 0);
      unzGoToNextFile(file);
    }
  return true;
}

}

}