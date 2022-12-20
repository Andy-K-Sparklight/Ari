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
Flow::run()
{
  if(tasks.size() == 0)
    {
      LOG("All task finished, no errors reported.");
      return;
    }
  FlowTask t = *(tasks.begin());
  tasks.pop_front();
  t(this, [&](int sig) -> void {
    if(sig == AL_ERR)
      {
        LOG("Flow received an err signal, last signal is " << output.back());
        completedStage = totalStage + 1; // Mark as failed and stop executing
      }
    else if(sig == AL_OK)
      {
        LOG("Flow received OK signal.");
        completedStage++;
        this->run(); // Run again for next task
      }
    else
      {
        LOG("Flow received signal " << sig);
        output.push_back(sig); // Collect signal
      }
  });
}

}
}