#include "ach/sys/Schedule.hh"

#include <list>
#include <functional>
#include <iostream>
#include <log.hh>
#define WEBVIEW_HEADER
#include <wv.hh>

namespace Alicorn
{
namespace Sys
{

uv_thread_t ACH_UV_THREAD;
uv_async_t ACH_UV_ASYNC_HANDLER;
bool uvRunningFlag = true;

static void
uvMain(void *ignored)
{
  while(uvRunningFlag)
    {
      uv_run(uv_default_loop(), UV_RUN_ONCE);
    }
}

typedef struct
{
  uv_rwlock_t lock;
  std::list<std::function<void()>> todos;
} UVAsyncCallerCarry;

// Runs on UV thread
// Those todos should be micro tasks, or the loop might get blocked
static void
uvAsyncCaller(uv_async_t *handler)
{
  // No worries if it has been called multiple times, since each call will
  // empty the todo list.
  UVAsyncCallerCarry *carry = (UVAsyncCallerCarry *)handler->data;
  uv_rwlock_rdlock(&carry->lock);
  while(carry->todos.size() > 0)
    {
      std::function<void()> f = *(carry->todos.begin());
      uv_rwlock_rdunlock(&carry->lock);
      f();

      uv_rwlock_wrlock(&carry->lock);
      carry->todos.pop_front();
      uv_rwlock_wrunlock(&carry->lock);
      uv_rwlock_rdlock(&carry->lock); // For next run
    }
  uv_rwlock_rdunlock(&carry->lock);
}

void
setupUVThread()
{
  LOG("Setting up UV thread.");
  ACH_UV_ASYNC_HANDLER.data = new UVAsyncCallerCarry;
  uv_rwlock_init(&((UVAsyncCallerCarry *)ACH_UV_ASYNC_HANDLER.data)->lock);
  uv_thread_create(&ACH_UV_THREAD, uvMain, (void *)NULL);
  uv_async_init(uv_default_loop(), &ACH_UV_ASYNC_HANDLER, uvAsyncCaller);
  LOG("UV thread created.");
}

void
runOnUVThread(std::function<void()> func)
{
  UVAsyncCallerCarry *carry = (UVAsyncCallerCarry *)ACH_UV_ASYNC_HANDLER.data;
  uv_rwlock_wrlock(&carry->lock);
  carry->todos.push_back(func);
  uv_async_send(&ACH_UV_ASYNC_HANDLER);
  uv_rwlock_wrunlock(&carry->lock);
}

typedef struct
{
  uv_thread_t tid;
  std::function<void()> x;
} WorkerData;

void
runOnWorkerThread(std::function<void()> func, bool join)
{
  auto tx = new WorkerData;
  tx->x = func;
  uv_thread_create(
      &tx->tid,
      [](void *a) -> void {
        auto t = (WorkerData *)a;
        t->x();
        delete t;
      },
      tx);
  if(join)
    {
      uv_thread_join(&tx->tid);
    }
}

void
stopUVThread()
{
  LOG("Stopping UV thread!");
  runOnUVThread([]() -> void {
    // The last run has completed, destroy the lock, close the caller.
    uv_close((uv_handle_t *)&ACH_UV_ASYNC_HANDLER, [](uv_handle_t *t) -> void {
      uv_rwlock_destroy(
          &((UVAsyncCallerCarry *)ACH_UV_ASYNC_HANDLER.data)->lock);
      LOG("Destroyed lock, UV loop is closing.");
      uvRunningFlag = false; // Avoid stopping the loop too early
      // The loop will stop itself, no need to use uv_stop.
      // Friendly!
    });
  });
  LOG("Waiting for UV thread to join...");
  uv_thread_join(&ACH_UV_THREAD); // Block and wait
  LOG("UV thread joined.");
}

void
runOnWorkerThreadMulti(std::function<std::function<void()>(int)> gen,
                       int times)
{
  WorkerData *threads[times];
  int i = 0;
  for(i = 0; i < times; i++)
    {
      auto wd = new WorkerData;
      wd->x = gen(i);
      uv_thread_create(
          &wd->tid,
          [](void *arg) -> void {
            auto t = (WorkerData *)arg;
            t->x();
            delete t;
          },
          wd);
      threads[i] = wd;
    }
  i--;
  for(; i >= 0; i--)
    {
      uv_thread_join(&threads[i]->tid);
    }
}

void
runOnMainThread(std::function<void()> func, void *window)
{
  auto fp = new std::function<void()>;
  *fp = func;
  webview_dispatch(
      window,
      [](webview_t w, void *fpa) -> void {
        auto fx = (std::function<void()> *)fpa;
        (*fx)();
        delete fx;
      },
      (void *)fp);
}

}
}