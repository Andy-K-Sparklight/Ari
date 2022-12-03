#include "ach/core/op/Flow.hh"

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
      return;
    }
  FlowTask t = *(tasks.begin());
  tasks.pop_front();
  t(this, [&](int sig) -> void {
    if(sig == AL_ERR)
      {
        completedStage = totalStage + 1; // Mark as failed and stop executing
      }
    else if(sig == AL_OK)
      {
        completedStage++;
        this->run(); // Run again for next task
      }
    else
      {
        output.push_back(sig); // Collect signal
      }
  });
}

}
}