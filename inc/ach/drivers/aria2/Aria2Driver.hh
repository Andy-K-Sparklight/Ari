#ifndef INCLUDE_ALICORN_CH_ACH_DRIVERS_ARIA2_ARIA2DRIVER
#define INCLUDE_ALICORN_CH_ACH_DRIVERS_ARIA2_ARIA2DRIVER

#include <uv.h>
#include <httplib.h>
#include <string>
#include <list>
#include <cJSON.h>

namespace AlicornDrivers
{
namespace Aria2
{

class Aria2Daemon
{
protected:
  uv_loop_t *uvLoop;
  uv_process_t proc; // The data member carries the stop flag
  uv_process_options_t optn = { 0 };
  unsigned int port = 6800;
  std::string token;
  bool stopFlag = false;
  friend void handleProcExit(uv_process_t *proc, int64_t exit_status,
                             int term_signal);

public:
  // Constructor
  Aria2Daemon();

  // Run daemon
  void run();

  // Stop daemon
  void stop();

  // Invoke command and get result
  std::string invoke(const std::string &method,
                     const std::list<cJSON *> &args);

  // Multicall version of invoke
  std::string invokeMulti(
      const std::list<std::pair<std::string, std::list<cJSON *> > > &commands);
};

extern Aria2Daemon ARIA2_DAEMON; // The global unique one

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_DRIVERS_ARIA2_ARIA2DRIVER */
