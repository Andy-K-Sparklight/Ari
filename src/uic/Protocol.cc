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
                      std::string *jsa = new std::string;
                      *jsa = js;
                      webview_dispatch(
                          mainWindow,
                          [](webview_t w, void *arg) -> void {
                            webview_eval(w, ((std::string *)arg)->c_str());
                          },
                          jsa);
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
  auto js = "if(window.a2Call)window.a2Call(`" + channel + "`,`" + dat + "`)";
  std::string *str = new std::string;
  *str = js;
  webview_dispatch(
      mainWindow,
      [](webview_t w, void *jsa) -> void {
        auto js = (std::string *)jsa;
        webview_eval(w, js->c_str());
        delete js;
      },
      str);
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
        l(s, cb);
        LISTENERS_CTL[channel] = NOP;
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