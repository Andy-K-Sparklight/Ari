#include "ach/util/Proc.hh"

#include <uv.h>

namespace Alicorn
{
namespace Commons
{

typedef struct
{
  uv_process_t proc;
  uv_process_options_t optn;
  uv_pipe_t pipe;
  uv_stdio_container_t cont[3];
  std::function<void(std::string)> cb;
  std::string data;
} ProcData;

// Alloc helper
static void
onAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

// Proc exit callback
static void
onExit(uv_process_t *p, int64_t code, int sig)
{
  uv_close((uv_handle_t *)p, [](uv_handle_t *h) -> void {
    auto p = (ProcData *)h->data;
    delete p;
  });
  uv_close((uv_handle_t *)&((ProcData *)p->data)->pipe, NULL);
}

void
onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{

  auto p = (ProcData *)stream->data;

  if(nread < 0 || nread == UV_EOF)
    {
      uv_read_stop(stream);
      p->cb(p->data);
    }
  else
    {
      std::string s(buf->base, nread);
      p->data += s;
    }
  if(buf->len > 0)
    {
      free(buf->base);
    }
}

void
runCommand(const std::string &bin, const std::list<std::string> &args,
           std::function<void(std::string)> cb, int pipe)
{
  ProcData *p = new ProcData;
  p->cb = cb;
  uv_pipe_init(uv_default_loop(), &p->pipe, 0);
  p->optn = { 0 };
  p->cont[0].flags = UV_IGNORE;
  p->cont[pipe].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  p->cont[3 - pipe].flags = UV_IGNORE;
  p->cont[pipe].data.stream = (uv_stream_t *)&p->pipe;
  p->optn.stdio = p->cont;
  p->optn.stdio_count = 3;
  p->optn.file = bin.c_str();
  p->optn.exit_cb = onExit;

  char *argsChar[args.size() + 2];
  argsChar[0] = new char[bin.length() + 1];
  strncpy(argsChar[0], bin.c_str(), bin.length() + 1);
  int i = 1;
  for(auto &a : args)
    {
      size_t len = a.length();
      argsChar[i] = new char[len + 1];
      strncpy(argsChar[i], a.c_str(), len + 1);
      i++;
    }
  argsChar[i] = NULL;
  p->optn.args = argsChar;

  p->proc.data = p; // Binding
  p->pipe.data = p;

  int r = uv_spawn(uv_default_loop(), &p->proc, &p->optn);
  if(r < 0)
    {
      cb("");
      uv_close((uv_handle_t *)&p->pipe, [](uv_handle_t *pipe) -> void {
        ProcData *pd = (ProcData *)pipe->data;
        delete pd;
      });
      return;
    }

  for(i -= 1; i >= 0; i--)
    {
      delete[] argsChar[i];
    }
  uv_read_start((uv_stream_t *)&p->pipe, onAlloc, onRead);
}

}
}