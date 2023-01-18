#ifndef INCLUDE_ALICORN_CH_ACH_UIC_PROTOCOL
#define INCLUDE_ALICORN_CH_ACH_UIC_PROTOCOL

#include <string>
#include <functional>

namespace Alicorn
{
namespace UIC
{
typedef std::function<void(const std::string &)> Callback;
typedef std::function<void(const std::string &, Callback)> Listener;

#define ACH_DEFAULT_PROGRESS                                                  \
  [](double p) -> void {                                                      \
    UIC::sendMessage("SetProgress", std::to_string(p * 100));                 \
  };

#define ACH_DEFAULT_STEP                                                      \
  [](int s) -> void { UIC::sendMessage("SetStep", std::to_string(s)); };

void initMainWindow(void *w);

void sendMessage(const std::string &channel, const std::string &dat);

void bindListener(const std::string &channel, Listener l, bool once = false);

void *getMainWindow();

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_PROTOCOL */
