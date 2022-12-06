#include "ach/drivers/aria2/Aria2Driver.hh"

#include <algorithm>
#include <csignal>
#include "ach/util/Exception.hh"

namespace AlicornDrivers
{
namespace Aria2
{

#define ACH_DRV_ARIA2_TOKEN_LENGTH 16

#define ACH_DRV_ARIA2_LISTEN_PORT "11451"

#if defined(_WIN32)
#define ACH_DRV_ARIA2_PROG_NAME "aria2c.prog"
#else
#define ACH_DRV_ARIA2_PROG_NAME "aria2c"
#endif

static std::string
randToken()
{
  srand(time(NULL));
  auto randchar = []() -> char {
    const char charset[]
        = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) / sizeof(char) - 1);
    return charset[rand() % max_index];
  };
  std::string str(ACH_DRV_ARIA2_TOKEN_LENGTH, 0);
  std::generate_n(str.begin(), ACH_DRV_ARIA2_TOKEN_LENGTH, randchar);
  return str;
}

Aria2Daemon::Aria2Daemon()
    : uvLoop(uv_default_loop()), optn({ 0 }), token(randToken())
{
  // Hold the pointer
  proc.data = this;
  proc.pid = 0;
}

void
handleProcExit(uv_process_t *proc, int64_t exit_status, int term_signal)
{
  proc->pid = 0;
  auto self = (Aria2Daemon *)proc->data;

  // Cleanup
  uv_close((uv_handle_t *)proc, NULL);

  // Reboot if necessary
  if(!self->stopFlag)
    {
      // Not ended yet
      self->run();
    }
};

void
Aria2Daemon::run()
{
  if(proc.pid != 0)
    {
      // Already running
      return;
    }

  char name[] = ACH_DRV_ARIA2_PROG_NAME;
  char enableRPC[] = "--enable-rpc=true";
  char rpcToken[14 + ACH_DRV_ARIA2_TOKEN_LENGTH] = "--rpc-secret=";
  char rpcPort[20 + sizeof(ACH_DRV_ARIA2_LISTEN_PORT) / sizeof(char)]
      = "--rpc-listen-port=";
  char maxConcurrent[] = "-j32";
  char memCache[] = "--disk-cache=32M";
  char downloadResult[] = "--max-download-result=32767";
  char concOptimize[] = "--optimize-concurrent-downloads=true";
  char allowOverwrite[] = "--allow-overwrite=true";
  char autoFileRenaming[] = "--auto-file-renaming=false";
  char maxConnection[] = "--max-connection-per-server=16";
  strcat(rpcToken, token.c_str());
  strcat(rpcPort, ACH_DRV_ARIA2_LISTEN_PORT);

  char *args[12];
  args[0] = name;
  args[1] = enableRPC;
  args[2] = rpcToken;
  args[3] = rpcPort;
  args[4] = maxConcurrent;
  args[5] = memCache;
  args[6] = downloadResult;
  args[7] = concOptimize;
  args[8] = allowOverwrite;
  args[9] = autoFileRenaming;
  args[10] = maxConnection;
  args[11] = NULL;

  optn.exit_cb = handleProcExit;
  optn.file = name;
  optn.args = args;

  int r;
  if((r = uv_spawn(uvLoop, &proc, &optn)))
    {
      throw Alicorn::Exception::ExternalException("Starting aria2c: ",
                                                  uv_strerror(r));
    }
}

void
Aria2Daemon::stop()
{
  stopFlag = true;
  uv_process_kill(&proc, SIGTERM); // Friendly!
}

static std::string
buildReqCall(const std::string &method, const std::list<cJSON *> &args,
             std::string &token)
{
  cJSON *req = cJSON_CreateObject();
  cJSON_AddStringToObject(req, "jsonrpc", "2.0");
  cJSON_AddStringToObject(req, "id", "ACH");
  cJSON_AddStringToObject(req, "method",
                          (std::string("aria2.") + method).c_str());
  cJSON *params = cJSON_CreateArray();
  cJSON *tk = cJSON_CreateString((std::string("token:") + token).c_str());

  cJSON_AddItemReferenceToArray(params, tk);
  for(auto &a : args)
    {
      cJSON_AddItemReferenceToArray(params, a);
    }
  cJSON_AddItemReferenceToObject(req, "params", params);
  std::string rpcCall = cJSON_Print(req);
  cJSON_Delete(tk);
  cJSON_Delete(params);
  cJSON_Delete(req);
  return rpcCall;
}

static std::string
sendRPCCall(const std::string &call)
{
  httplib::Client rpcClient(std::string("http://localhost:")
                            + ACH_DRV_ARIA2_LISTEN_PORT);

  auto res = rpcClient.Post("/jsonrpc", call, "application/json");
  if(!res)
    {
      throw Alicorn::Exception::ExternalException("Aria2 RPC response: ",
                                                  "Failed to connect");
    }
  if(res->status != 200)
    {

      throw Alicorn::Exception::ExternalException("Aria2 RPC response: ",
                                                  "" + res->status);
    }
  else
    {
      return res->body;
    }
}

std::string
Aria2Daemon::invoke(const std::string &method, const std::list<cJSON *> &args)
{
  return sendRPCCall(buildReqCall(method, args, token));
}
std::string
Aria2Daemon::invokeMulti(
    const std::list<std::pair<std::string, std::list<cJSON *> > > &commands)
{
  if(commands.size() == 0)
    {
      return "";
    }
  std::string arrStr = "[";
  for(auto &c : commands)
    {
      arrStr += buildReqCall(c.first, c.second, token);
      arrStr += ",";
    }
  arrStr.pop_back();
  arrStr += "]";

  return sendRPCCall(arrStr);
}

Aria2Daemon ARIA2_DAEMON; // The global unique one

void
driverInit()
{
  ARIA2_DAEMON.run();
}
}
}