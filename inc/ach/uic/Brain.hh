#ifndef INCLUDE_ALICORN_CH_ACH_UIC_BRAIN
#define INCLUDE_ALICORN_CH_ACH_UIC_BRAIN

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <list>
#include "ach/uic/UIFrame.hh"

namespace Alicorn
{
namespace UIC
{

typedef std::map<std::string, std::string> DataPool;

enum InstrVerb
{
  NOP, // No operation
  RET, // Return
  JMP, // Jump to label
  RUN, // Run an action
  SYS, // System call
  WDG, // Append widget
  CMP, // Compare value
  JC,  // Jump if carry (success)
  JNC, // Jump if !carry
  ENT, // Append entry using previous settings
  UIC, // Wait for user input
  UIP, // Display UI, but do not wait for user
  LN,  // Link this choice with a label in program
  PSH, // Push into stack
  POP, // Pop from stack
  MOV, // Copy data
  PP,  // Assign property
  CR,  // Set carry flag
  HLT, // Halt, no more ops
  PUT, // Debug output
  AC,  // Acknowledge variable for frontend
};

typedef struct
{
  unsigned int verb;
  std::string a1, a2;
} Instr;

class Program
{
public:
  unsigned int eip = 0;
  std::string name;
  std::vector<Instr> bin;
  std::list<std::string> stack;
  std::map<std::string, unsigned int> labels;
  UIFrame frame;

  DataPool locals;
  DataPool *storage; // System provided storage area
  bool carry;
  std::string resolveValue(const std::string &label);
  std::string &resolveValueRef(const std::string &label);

  Program(const std::string &src, const std::string &name, DataPool *storage);
  void run(PCallback cb);
};

void runProgram(const std::string &name, PCallback cb, DataPool *data);

typedef std::function<void(Program &, PCallback cb)> PSysCall;

void bindSysCall(const std::string &name, PSysCall fun);

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_BRAIN */
