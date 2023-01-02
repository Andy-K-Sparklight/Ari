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

int
main(int argc, char **argv)
{
  Alicorn::Commons::argv0 = argv[0];
  std::vector<std::string> args;
  for(int i = 0; i < argc; i++)
    {
      args.push_back(argv[i]);
    }

  webview_t w = webview_create(true, nullptr);
  webview_set_size(w, 960, 540, WEBVIEW_HINT_NONE);
  webview_bind(
      w, "tellSize",
      [](const char *seq, const char *req, void *arg) -> void {
        webview_t loginWindow = (webview_t)arg;
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
          }
        int aw = (960.0 / vw) * scrnW * 0.6;
        int ah = (540.0 / vh) * scrnH * 0.6;
        webview_set_size(loginWindow, aw, ah, WEBVIEW_HINT_NONE);
        cJSON_Delete(a);
      },
      w);

  if(args[1] == "mslogin")
    {
      // Start login
      webview_navigate(
          w, "https://login.live.com/"
             "oauth20_authorize.srf?client_id=00000000402b5328&response_type="
             "code&scope=service%3A%3Auser.auth.xboxlive.com%3A%3AMBI_SSL&"
             "redirect_uri=https%3A%2F%2Flogin.live."
             "com%2Foauth20_desktop.srf");
      webview_init(
          w,
          "window.onload=function(){window.tellSize(screen.availWidth,screen."
          "availHeight,window.outerWidth,window.outerHeight);var "
          "u=window.location.href;if(u.includes(\"code=\")){u=u.split(\"code="
          "\")[1];if(u.includes(\"&lc=\"))u=u.split(\"&lc=\")[0];window."
          "tellCode(u);}}");
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
            webview_terminate(loginWindow);
          },
          w);
      webview_run(w);
    }
  else
    {
      // Run init
      using namespace Alicorn;
      UIC::initMainWindow(w);
      webview_set_title(w, "A2 | GREAT A2");
      UIC::bindListener("Ping",
                        [](const std::string &s, UIC::Callback cb) -> void {
                          cb("\"...It's courage.\"");
                        });

      // Load Script
      std::ifstream jsf("Main.js");
      std::stringstream jss;
      jss << jsf.rdbuf();
      std::string js = jss.str();
      Sys::initSys();
      auto boot = "window.onload=()=>{" + js + "}";
      webview_init(w, boot.c_str());
      webview_set_html(w, "<!DOCTYPE html><html><body><div "
                          "id=\"a2root\"></"
                          "div></body></html>");
      webview_run(w);
      Sys::downSys();
    }

  return 0;
}