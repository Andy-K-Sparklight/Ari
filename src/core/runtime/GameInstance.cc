#include "ach/core/runtime/GameInstance.hh"

#include <cstring>
#include <csignal>
#include "ach/util/Exception.hh"

namespace Alicorn
{
namespace Runtime
{

// Alloc helper
static void
onAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

// Read output callback
static void
onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
  if(nread < 0 || nread == UV_EOF) // Actually EOF < 0, just do this explictly
    {
      uv_read_stop(stream); // Stop reading
      uv_close((uv_handle_t *)stream, NULL);
      return;
    }
  if(nread > 0)
    {
      GameInstance *self = (GameInstance *)stream->data;
      char tmp[nread + 1];
      strncpy(tmp, buf->base, nread + 1);
      tmp[nread] = 0; // Terminate it

      // Read two streams to one buffer
      // TODO: I guess this can hardly cause any trouble, but I'll check
      self->outputBuf.push_back(std::string(tmp));
    }
  if(buf->len > 0)
    {
      free(buf->base);
    }
}

// Proc exit callback
static void
onExit(uv_process_t *proc, int64_t code, int sig)
{
  GameInstance *self = (GameInstance *)proc->data;

  // Cleanup
  uv_close((uv_handle_t *)proc, NULL);
  if(self->stat != GS_KILLED)
    {
      if(code == 0)
        {
          self->stat = GS_ENDED;
        }
      else
        {
          self->stat = GS_CRASHED;
        }
    }

  // Gracefully
  uv_read_stop((uv_stream_t *)&self->outPipes[0]);
  uv_read_stop((uv_stream_t *)&self->outPipes[1]);
  uv_close((uv_handle_t *)&self->outPipes[0], NULL);
  uv_close((uv_handle_t *)&self->outPipes[1], NULL);
}

void
GameInstance::run()
{
  if(stat != GS_LOADED)
    {
      return; // Already running
    }
  proc.data = this; // Binding

  // Prepare for IO
  uv_pipe_init(uv_default_loop(), &outPipes[0], 0);
  uv_pipe_init(uv_default_loop(), &outPipes[1], 0);

  ioContainer[0].flags = UV_IGNORE;
  ioContainer[1].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  ioContainer[2].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  ioContainer[1].data.stream = (uv_stream_t *)&outPipes[0]; // stdout
  ioContainer[2].data.stream = (uv_stream_t *)&outPipes[1]; // stderr

  options.stdio = ioContainer;
  options.stdio_count = 3;
  options.flags = UV_PROCESS_DETACHED; // Don't be evil...
  options.exit_cb = onExit;

  // Binary exec
  options.file = bin.c_str();

  // Args processing
  char *argsChar[args.size() + 2]; // One reserved for argv0, one for NULL
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

  options.args = argsChar;

  // Spawning
  int r = uv_spawn(uv_default_loop(), &proc, &options);

  // Cleanup
  for(i -= 1; i >= 0; i--)
    {
      delete argsChar[i];
    }

  if(r)
    {
      stat = GS_FAILED;
      throw Exception::ExternalException("Spawning game process: ",
                                         uv_strerror(r));
    }

  stat = GS_LAUNCHED;

  // Setup reading
  uv_read_start((uv_stream_t *)&outPipes[0], onAlloc, onRead);
  uv_read_start((uv_stream_t *)&outPipes[1], onAlloc, onRead);
}

void
GameInstance::stop()
{
  if(stat != GS_LAUNCHED && stat != GS_PLAYING)
    {
      return;
    }
  uv_process_kill(&proc, SIGTERM); // Be nice!
}

void
GameInstance::kill()
{
  if(stat != GS_LAUNCHED && stat != GS_PLAYING)
    {
      return;
    }
  uv_process_kill(&proc, SIGKILL); // If you wish...
}

}
}