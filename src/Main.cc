#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <wv.hh>
#include <cJSON.h>
#include "ach/util/Commons.hh"
#include "ach/sys/Init.hh"
#include <unistd.h>

int
main(int argc, char **argv)
{
  Alicorn::Commons::argv0 = argv[0];
  std::vector<std::string> args;
  for(int i = 0; i < argc; i++)
    {
      args.push_back(argv[i]);
    }
  if(args[1] == "mslogin")
    {
      // Start login
      webview_t loginWindow = webview_create(false, nullptr);
      webview_set_size(loginWindow, 960, 540, WEBVIEW_HINT_NONE);

      webview_navigate(
          loginWindow,
          "https://login.live.com/"
          "oauth20_authorize.srf?client_id=00000000402b5328&response_type="
          "code&scope=XboxLive.signin&redirect_uri=https%3A%2F%2Flogin.live."
          "com%2Foauth20_desktop.srf");
      webview_init(
          loginWindow,
          "window.onload=function(){window.tellSize(screen.availWidth, "
          "screen.availHeight, window.outerWidth, window.outerHeight);let "
          "code=window.location.href.match(/(?<=code=)[^&]+/"
          ");if(code!=null){window.tellCode(code[0]);}};");

      webview_bind(
          loginWindow, "tellSize",
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
          loginWindow);
      webview_bind(
          loginWindow, "tellCode",
          [](const char *seq, const char *req, void *arg) -> void {
            webview_t loginWindow = (webview_t)arg;
            cJSON *params = cJSON_Parse(req);
            if(cJSON_IsArray(params))
              {
                cJSON *code = cJSON_GetArrayItem(params, 1);
                if(cJSON_IsString(code))
                  {
                    std::cerr << cJSON_GetStringValue(code) << std::endl;
                  }
              }
            webview_terminate(loginWindow);
          },
          loginWindow);
      webview_run(loginWindow);
    }
  else
    {
      // Run init
    }

  return 0;
}