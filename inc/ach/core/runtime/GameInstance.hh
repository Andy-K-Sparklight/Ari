#ifndef INCLUDE_ALICORN_CH_ACH_CORE_RUNTIME_GAMEINSTANCE
#define INCLUDE_ALICORN_CH_ACH_CORE_RUNTIME_GAMEINSTANCE

#include <string>
#include <list>
#include <sstream>
#include <uv.h>
#include <map>

namespace Alicorn
{
namespace Runtime
{

enum GameInstanceStatus
{
  GS_LOADED,   // Ready to be launched
  GS_LAUNCHED, // Launched, but game not ready
  GS_PLAYING,  // Ready for play, mainly guessed from logs
  GS_CLOSING,  // Shutting down
  GS_ENDED,    // Ended normally
  GS_FAILED,   // Failed to launch
  GS_KILLED,   // Forcefully killed
  GS_CRASHED,  // Crashed and ended
};

class GameInstance
{
public:
  std::string bin;
  std::list<std::string> args;
  uv_process_t proc;
  uv_stdio_container_t ioContainer[3];
  uv_pipe_t outPipes[2];       // The output of stdout and stderr
  std::stringstream outputBuf; // Buffer
  uv_process_options_t options = { 0 };
  GameInstanceStatus stat = GS_LOADED;
  std::string cwd;

  // Create process with specified options
  // Always run this on UV thread!
  bool run();

  // Stop the process normally
  void stop();

  // Kill the process forcefully
  void kill();
};

// A map of all instances (PID -> Instance)
// Should be removed manually
extern std::map<int, GameInstance *> GAME_INSTANCES;

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_CORE_RUNTIME_GAMEINSTANCE */
