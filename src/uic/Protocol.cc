#include "ach/uic/Protocol.hh"

#include <map>
#include <cJSON.h>
#include <log.hh>
#include <algorithm>
#include "ach/sys/Schedule.hh"
#define WEBVIEW_HEADER
#include <wv.hh>

namespace Alicorn
{
namespace UIC
{

static webview_t mainWindow;
static bool mainWindowReady = false;
static std::map<std::string, Listener> LISTENERS_CTL;

void
initMainWindow(void *w)
{
  if(w == nullptr)
    {
      mainWindowReady = false;
      return;
    }
  mainWindow = (webview_t)w;
  webview_bind(
      mainWindow, "SYS",
      [](const char *seq, const char *req, void *arg) -> void {
        auto args = cJSON_Parse(req);
        if(cJSON_IsArray(args))
          {
            auto chn = cJSON_GetArrayItem(args, 0);
            auto pid = cJSON_GetArrayItem(args, 1);
            auto par = cJSON_GetArrayItem(args, 2);
            int pidInt = (int)cJSON_GetNumberValue(pid);
            if(cJSON_IsString(chn))
              {
                auto chnStr = std::string(cJSON_GetStringValue(chn));
                // Tweak to set ready flag
                if(chnStr == "Ready")
                  {
                    mainWindowReady = true;
                  }

                auto fn = LISTENERS_CTL[chnStr];
                if(fn != nullptr)
                  {
                    auto parStr = cJSON_GetStringValue(par);
                    Callback cb = [=](const std::string &ret) -> void {
                      std::string js = "if(window.a2Reply)window.a2Reply(`"
                                       + ret + "`," + std::to_string(pidInt)
                                       + ")";
                      Sys::runOnMainThread(
                          [js]() -> void {
                            webview_eval(mainWindow, js.c_str());
                          },
                          mainWindow);
                    };
                    if(parStr != NULL)
                      {
                        fn(std::string(parStr), cb);
                      }
                    else
                      {
                        fn("", cb);
                      }
                  }
              }
          }
        cJSON_Delete(args);
      },
      NULL);
}

void
sendMessage(const std::string &channel, const std::string &dat)
{
  if(mainWindowReady)
    {
      std::string datr = dat;
      size_t start_pos = 0;
      while((start_pos = datr.find("\\", start_pos)) != std::string::npos)
        {
          datr = datr.replace(start_pos, 1, "\\\\");
          start_pos += 2;
        }
      auto js
          = "if(window.a2Call)window.a2Call(`" + channel + "`,`" + datr + "`)";

      Sys::runOnMainThread(
          [js]() -> void { webview_eval(mainWindow, js.c_str()); },
          mainWindow);
    }
}

static Listener NOP = [](const std::string &s, Callback cb) -> void {};

void
bindListener(const std::string &channel, Listener l, bool once)
{
  if(!once)
    {
      LISTENERS_CTL[channel] = l;
    }
  else
    {
      LISTENERS_CTL[channel]
          = [l, channel](const std::string &s, Callback cb) -> void {
        LISTENERS_CTL[channel] = NOP;
        l(s, cb);
      };
    }
}

void *
getMainWindow()
{
  return mainWindow;
}

}
}