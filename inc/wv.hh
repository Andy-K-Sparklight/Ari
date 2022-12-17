#ifndef INCLUDE_ALICORN_CH_WV
#define INCLUDE_ALICORN_CH_WV

#ifdef __APPLE__

// A header-only part for macOS

#define WEBVIEW_HINT_NONE 0
#define WEBVIEW_HINT_MIN 1
#define WEBVIEW_HINT_MAX 2
#define WEBVIEW_HINT_FIXED 3

extern "C"
{
  typedef void *webview_t;
  webview_t webview_create(int debug, void *window);
  void webview_destroy(webview_t w);
  void webview_run(webview_t w);
  void webview_terminate(webview_t w);
  void webview_dispatch(webview_t w, void (*fn)(webview_t w, void *arg),
                        void *arg);
  void *webview_get_window(webview_t w);
  void webview_set_title(webview_t w, const char *title);
  void webview_set_size(webview_t w, int width, int height, int hints);
  void webview_navigate(webview_t w, const char *url);
  void webview_set_html(webview_t w, const char *html);
  void webview_init(webview_t w, const char *js);
  void webview_eval(webview_t w, const char *js);
  void webview_bind(webview_t w, const char *name,
                    void (*fn)(const char *seq, const char *req, void *arg),
                    void *arg);
  void webview_unbind(webview_t w, const char *name);
  void webview_return(webview_t w, const char *seq, int status,
                      const char *result);
}

#else
#include <webview.h>
#endif
#endif /* INCLUDE_ALICORN_CH_WV */
