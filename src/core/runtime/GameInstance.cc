#include "ach/core/runtime/GameInstance.hh"

#include <cstring>
#include <csignal>
#include <iostream>
#include <cstdlib>
#include <log.hh>
#include <filesystem>
#include <list>
#include <vector>

extern char **environ;

namespace Alicorn
{
namespace Runtime
{

#define ACH_NVENV_COUNT 3
static std::vector<std::string> nvEnv
    = { "__NV_PRIME_RENDER_OFFLOAD=1", "__VK_LAYER_NV_optimus=NVIDIA_only",
        "__GLX_VENDOR_LIBRARY_NAME=nvidia" };

// Get all environment variables as list

static std::list<std::string>
getAllEnvs()
{
  std::list<std::string> o;
  for(char **current = environ; *current; current++)
    {
      o.push_back(std::string(*current));
    }
  return o;
}

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
      // DO NOT close here they will be closed on exit cb
      return;
    }
  if(nread > 0)
    {
      GameInstance *self = (GameInstance *)stream->data;
      std::string ln = std::string(buf->base, nread);
      self->outputBuf << ln;
      std::cout << ln;
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
  LOG("Game process exited with CODE " << code << " and SIG " << sig);

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
  LOG("Closing related streams for game.");
  uv_read_stop((uv_stream_t *)&self->outPipes[0]);
  uv_read_stop((uv_stream_t *)&self->outPipes[1]);
  uv_close((uv_handle_t *)&self->outPipes[0], NULL);
  uv_close((uv_handle_t *)&self->outPipes[1], NULL);
  if(self->exitListener != nullptr)
    {
      self->exitListener();
    }
}

// Not reusable!
bool
GameInstance::run()
{

  if(stat != GS_LOADED)
    {
      return false; // Already running
    }
  proc.data = this; // Binding

  // Prepare for IO
  LOG("Preparing to launch game.");
  uv_pipe_init(uv_default_loop(), &outPipes[0], 0);
  uv_pipe_init(uv_default_loop(), &outPipes[1], 0);

  ioContainer[0].flags = UV_IGNORE;
  ioContainer[1].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  ioContainer[2].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  ioContainer[1].data.stream = (uv_stream_t *)&outPipes[0]; // stdout
  ioContainer[2].data.stream = (uv_stream_t *)&outPipes[1]; // stderr

  outPipes[0].data = this; // Bindings
  outPipes[1].data = this;

  options.stdio = ioContainer;
  options.stdio_count = 3;
  options.flags = UV_PROCESS_DETACHED; // Don't be evil...
  options.exit_cb = onExit;

  // Binary exec
  options.file = bin.c_str();

  if(cwd.size() > 0)
    {
      options.cwd = cwd.c_str();
    }

  // Args processing

  char *argsChar[args.size() + 2]; // One reserved for argv0, one for NULL
  argsChar[0] = new char[bin.size() + 1];
  strncpy(argsChar[0], bin.c_str(), bin.length() + 1);
  int i = 1;
  for(auto &a : args)
    {
      size_t len = a.size();
      argsChar[i] = new char[len + 1];
      strncpy(argsChar[i], a.c_str(), len + 1);
      i++;
    }
  argsChar[i] = NULL;
  LOG(args.size() << " args applied.");
  options.args = argsChar;

  auto envs = getAllEnvs();
  char *envChar[envs.size() + 4]; // 3 more vars, 1 NULL
  int t = 0;
  if(std::filesystem::exists("/usr/bin/prime-run"))
    {
      int j = 0;
      size_t k = 0;
      for(auto &v : envs)
        {
          size_t len = v.size();
          envChar[j] = new char[len + 1];
          strncpy(envChar[j], v.c_str(), len + 1);
          j++;
        }
      for(; k < nvEnv.size(); k++)
        {
          auto va = nvEnv[k];
          size_t len = va.size();
          envChar[j + k] = new char[len + 1];
          strncpy(envChar[j + k], va.c_str(), len + 1);
        }
      envChar[j + k] = NULL;
      options.env = envChar;
      t = j + k;
    }

  // Spawning
  LOG("Spawning process.");
  int r = uv_spawn(uv_default_loop(), &proc, &options);

  // Cleanup
  for(i -= 1; i >= 0; i--)
    {
      delete[] argsChar[i];
    }
  for(t -= 1; t >= 0; t--)
    {
      delete[] envChar[t];
    }
  if(r != 0)
    {
      LOG("Failed to spawn game process: " << uv_strerror(r));
      stat = GS_FAILED;
      uv_close((uv_handle_t *)&outPipes[0], NULL);
      uv_close((uv_handle_t *)&outPipes[1], NULL);
      return false;
    }

  LOG("Spawned game process with pid " << proc.pid);
  stat = GS_LAUNCHED;
  // Setup reading
  uv_read_start((uv_stream_t *)&outPipes[0], onAlloc, onRead);
  uv_read_start((uv_stream_t *)&outPipes[1], onAlloc, onRead);
  LOG("Started piping game output.");
  return true;
}

void
GameInstance::stop()
{
  if(stat != GS_LAUNCHED && stat != GS_PLAYING)
    {
      return;
    }
  LOG("Stopping game process " << proc.pid << " gracefully.");
  uv_process_kill(&proc, SIGTERM); // Be nice!
}

void
GameInstance::kill()
{
  if(stat != GS_LAUNCHED && stat != GS_PLAYING)
    {
      return;
    }
  LOG("Stopping game process " << proc.pid << " forcefullly!");
  uv_process_kill(&proc, SIGKILL); // If you wish...
}

std::map<int, GameInstance *> GAME_INSTANCES;
}
}