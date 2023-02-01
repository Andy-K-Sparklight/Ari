#include "ach/drivers/aria2/Aria2Driver.hh"

#include <algorithm>
#include <csignal>
#include <log.hh>

namespace AlicornDrivers
{
namespace Aria2
{

#define ACH_DRV_ARIA2_TOKEN_LENGTH 16

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

Aria2Daemon::Aria2Daemon() : optn({ 0 }), token(randToken())
{
  // Hold the pointer
  proc.data = this;
  proc.pid = 0;
}

void
handleProcExit(uv_process_t *proc, int64_t exit_status, int term_signal)
{
  // Cleanup
  uv_close((uv_handle_t *)proc, [](uv_handle_t *proc) -> void {
    ((uv_process_t *)proc)->pid = 0; // Assign 0 PID
    auto self = (Aria2Daemon *)proc->data;
    // Reboot if necessary
    if(!self->stopFlag)
      {
        // Not ended yet
        self->run();
      }
  });
};

bool
Aria2Daemon::run()
{
  if(proc.pid != 0)
    {
      // Already running
      return false;
    }

  port++;
  if(port >= 65536)
    {
      port = 1025;
    }

  auto cPort = std::to_string(port);
  char name[] = ACH_DRV_ARIA2_PROG_NAME;
  char enableRPC[] = "--enable-rpc=true";
  char rpcToken[14 + ACH_DRV_ARIA2_TOKEN_LENGTH];
  strcpy(rpcToken, "--rpc-secret=");
  char rpcPort[19 + cPort.size()];
  strcpy(rpcPort, "--rpc-listen-port=");
  char maxConcurrent[] = "-j32";
  char memCache[] = "--disk-cache=32M";
  char downloadResult[] = "--max-download-result=32767";
  char concOptimize[] = "--optimize-concurrent-downloads=true";
  char allowOverwrite[] = "--allow-overwrite=true";
  char autoFileRenaming[] = "--auto-file-renaming=false";
  char maxConnection[] = "--max-connection-per-server=16";
  strcat(rpcToken, token.c_str());
  strcat(rpcPort, cPort.c_str());

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
  LOG("Starting aria2 daemon with port " << port);
  int r;
  if((r = uv_spawn(uv_default_loop(), &proc, &optn)))
    {
      LOG("Could not start aria2 daemon: " << uv_strerror(r));
      return false;
    }
  LOG("Successfully started aria2 daemon with pid " << proc.pid);
  return true;
}

void
Aria2Daemon::stop()
{
  LOG("Stopping aria2 daemon!");
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
sendRPCCall(const std::string &call, unsigned int port)
{
  httplib::Client rpcClient(std::string("http://localhost:")
                            + std::to_string(port));
  rpcClient.set_follow_location(true);
  auto res = rpcClient.Post("/jsonrpc", call, "application/json");
  if(res == nullptr || res->status != 200)
    {
      LOG("Aria2 responded with unexpected result!");
      return "";
    }
  else
    {
      return res->body;
    }
}

std::string
Aria2Daemon::invoke(const std::string &method, const std::list<cJSON *> &args)
{
  return sendRPCCall(buildReqCall(method, args, token), port);
}
std::string
Aria2Daemon::invokeMulti(
    const std::list<std::pair<std::string, std::list<cJSON *>>> &commands)
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

  return sendRPCCall(arrStr, port);
}

Aria2Daemon *ARIA2_DAEMON; // The global unique one

}
}
