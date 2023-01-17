#include "ach/core/op/Flow.hh"

#include "log.hh"

namespace Alicorn
{
namespace Op
{

void
Flow::addTask(FlowTask t)
{
  tasks.push_back(t);
  totalStage++;
}

void
Flow::run(std::function<void(bool)> cb)
{
  if(tasks.size() == 0)
    {
      LOG("All task finished, no errors reported.");
      if(cb != nullptr)
        {
          cb(true);
        }
      return;
    }
  onProgress(-1); // Set as indefinite first
  FlowTask t = *(tasks.begin());
  tasks.pop_front();
  t(this, [&, cb](int sig) -> void {
    if(sig == AL_ERR)
      {
        LOG("Flow received an err signal, last signal is " << output.back());
        completedStage = totalStage + 1; // Mark as failed and stop executing
        if(cb != nullptr)
          {
            cb(false);
          }
      }
    else if(sig == AL_OK)
      {
        completedStage++;
        this->onProgress(-1); // Reset progress
        this->run(cb);        // Run again for next task
      }
    else
      {
        output.push_back(sig); // Collect signal
      }
  });
}

}
}