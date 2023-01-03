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

void initMainWindow(void *w);

void sendMessage(const std::string &channel, const std::string &dat);

void bindListener(const std::string &channel, Listener l, bool once = false);

void *getMainWindow();

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_PROTOCOL */
