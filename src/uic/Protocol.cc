#include "ach/uic/Protocol.hh"

#define WEBVIEW_HEADER
#include <wv.hh>
#include <map>
#include <cJSON.h>
#include <log.hh>

namespace Alicorn
{
namespace UIC
{

static webview_t mainWindow;
static std::map<std::string, Listener> LISTENERS_CTL;

void
initMainWindow(void *w)
{
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
                auto fn
                    = LISTENERS_CTL[std::string(cJSON_GetStringValue(chn))];
                if(fn != nullptr)
                  {
                    auto parStr = cJSON_GetStringValue(par);
                    Callback cb = [=](const std::string &ret) -> void {
                      std::string js = "if(window.a2Reply)window.a2Reply(`"
                                       + ret + "`," + std::to_string(pidInt)
                                       + ")";
                      webview_eval(mainWindow, js.c_str());
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
  auto js
      = "if(window.a2Call)window.a2Call(\"" + channel + "\",\"" + dat + "\")";
  webview_eval(mainWindow, js.c_str());
}

void
bindListener(const std::string &channel, Listener l)
{
  LISTENERS_CTL[channel] = l;
}

}
}