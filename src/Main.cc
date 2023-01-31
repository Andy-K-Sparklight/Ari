#define ACH_USE_TERMINALUI

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "ach/util/Commons.hh"
#include "ach/sys/Init.hh"
#include "ach/uic/Protocol.hh"
#include <wv.hh>
#include <cJSON.h>
#include <log.hh>
#include "ach/uic/Brain.hh"
#include "ach/sys/Schedule.hh"
#include "ach/uic/UserData.hh"
#include "ach/core/platform/OSType.hh"

int
main(int argc, char **argv)
{
  Alicorn::Commons::argv0 = argv[0];
  std::vector<std::string> args;
  for(int i = 0; i < argc; i++)
    {
      std::string xi = argv[i];
      args.push_back(xi);
    }
  if(argc <= 1)
    {
      args.push_back("");
    }

  webview_t w = webview_create(true, nullptr);

  // Configure size
  if(args[1] != "configure")
    {
      for(;;)
        {
          Alicorn::UIC::loadUserData();
          auto udm = Alicorn::UIC::getUserData();
          auto wx = (*udm)["$$Width"];
          auto hx = (*udm)["$$Height"];
          if(wx.size() > 0 && hx.size() > 0)
            {
              double scale = argc <= 1 ? 0.6 : 0.8;
              webview_set_size(w, std::stoi(wx) * scale, std::stoi(hx) * scale,
                               WEBVIEW_HINT_NONE);

              break;
            }
          else
            {
              system(
                  ("\"" + Alicorn::Commons::argv0 + "\" configure").c_str());
            }
        }
    }

  if(args[1] == std::string("mslogin"))
    {
      // Start login
      webview_init(
          w, "window.onload=function(){var "
             "u=window.location.href;if(u.includes(\"code=\")){u=u.split("
             "\"code="
             "\")[1];if(u.includes(\"&lc=\"))u=u.split(\"&lc=\")[0];window."
             "tellCode(u);}}");
      webview_navigate(
          w, "https://login.live.com/"
             "oauth20_authorize.srf?client_id=00000000402b5328&response_type="
             "code&scope=service%3A%3Auser.auth.xboxlive.com%3A%3AMBI_SSL&"
             "redirect_uri=https%3A%2F%2Flogin.live."
             "com%2Foauth20_desktop.srf");
      webview_set_title(w, "Login with Microsoft");
      webview_bind(
          w, "tellCode",
          [](const char *seq, const char *req, void *arg) -> void {
            webview_t loginWindow = (webview_t)arg;
            cJSON *params = cJSON_Parse(req);
            if(cJSON_IsArray(params))
              {
                cJSON *code = cJSON_GetArrayItem(params, 0);
                if(cJSON_IsString(code))
                  {
                    std::cout << cJSON_GetStringValue(code) << std::endl;
                  }
              }
            cJSON_Delete(params);
            webview_dispatch(
                loginWindow,
                [](webview_t w, void *) -> void {
                  webview_destroy(w);
                  webview_terminate(w);
                },
                loginWindow);
          },
          w);
      webview_run(w);
    }
  else if(args[1] == std::string("modrinth"))
    {
      // Load modrinth
      std::ifstream jsf("ModrinthTweak.js");
      std::stringstream jss;
      jss << jsf.rdbuf();
      std::string js = jss.str();
      webview_set_title(w, "Modrinth");
      webview_init(w, ("window.onload=function(){" + js + "}").c_str());
      webview_navigate(w, "https://modrinth.com/mods");
      webview_run(w);
      return 0;
    }
  else if(args[1] == std::string("configure"))
    {
      using namespace Alicorn;
      UIC::loadUserData();
      webview_set_title(w, "Ari Configurator");
      webview_set_size(w, 960, 540, WEBVIEW_HINT_NONE);
      webview_bind(
          w, "cfgSize",
          [](const char *seq, const char *req, void *arg) -> void {
            auto w = (webview_t)arg;
            int scrnW = 1920, scrnH = 1080, vw = 960, vh = 540;
            cJSON *a = cJSON_Parse(req);
            if(cJSON_IsArray(a))
              {
                if(cJSON_GetArraySize(a) != 4)
                  {
                    cJSON_Delete(a);
                    return;
                  }
                scrnW = cJSON_GetNumberValue(cJSON_GetArrayItem(a, 0));
                scrnH = cJSON_GetNumberValue(cJSON_GetArrayItem(a, 1));
                vw = cJSON_GetNumberValue(cJSON_GetArrayItem(a, 2));
                vh = cJSON_GetNumberValue(cJSON_GetArrayItem(a, 3));
                int aw = (960.0 / vw) * scrnW;
                int ah = (540.0 / vh) * scrnH;
                (*UIC::getUserData())["$$Width"] = std::to_string(aw);
                (*UIC::getUserData())["$$Height"] = std::to_string(ah);
                UIC::saveUserData();
                webview_dispatch(
                    w,
                    [](webview_t w, void *) -> void {
                      webview_destroy(w);
                      webview_terminate(w);
                    },
                    w);
              }
            cJSON_Delete(a);
          },
          w);
      webview_init(
          w, "window.onload=()=>{requestIdleCallback(()=>{window.cfgSize("
             "screen.availWidth,screen."
             "availHeight,window.outerWidth,window.outerHeight);});}");
      webview_navigate(w, "about:blank");
      webview_run(w);
      return 0;
    }
  else
    {
      // Run init
      using namespace Alicorn;
      Sys::initSys();
      UIC::initMainWindow(w);
      webview_set_title(w, "Ari 2023");
      UIC::bindListener("Ping",
                        [](const std::string &s, UIC::Callback cb) -> void {
                          cb("\"...It's courage.\"");
                        });
      UIC::bindListener(
          "Ready",
          [w](const std::string &dat, UIC::Callback cb) -> void {
            LOG("Start executing main entry script.");
            Sys::runOnWorkerThread([w]() -> void {
              UIC::runProgram(
                  "Main", []() -> void {}, UIC::getUserData());
            });
            cb("");
          },
          true);

      // Load Script
      std::ifstream jsf("Main.js");
      std::stringstream jss;
      jss << jsf.rdbuf();
      std::string js = jss.str();
      auto boot = "window.addEventListener('load', ()=>{" + js + "});";
      webview_init(w, boot.c_str());
      if(Platform::OS_TYPE == Platform::OS_DARWIN)
        {
          webview_navigate(w, "about:blank");
        }
      else
        {
          webview_navigate(w,
                           "data:text/html,<!DOCTYPE "
                           "html><html><body><style>body{text-align:center;} "
                           "h1{width:100%;} p{width:100%;}</"
                           "style><h1>Please Wait...</h1><p>Loading "
                           "components</p></body></html>");
        }
      webview_run(w);
      Sys::saveData();
      Sys::downSys();
    }

  return 0;
}