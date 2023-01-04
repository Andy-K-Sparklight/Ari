#include "ach/uic/Brain.hh"

#include "ach/util/Commons.hh"
#include <fstream>
#include <sstream>
#include <log.hh>
#include <regex>

namespace Alicorn
{
namespace UIC
{

static std::map<std::string, PSysCall> SYSCALLCTL;

static std::string
resolveName(const std::string &name)
{
  return "brain/" + name + ".a2s";
}

static std::map<std::string, unsigned int> VERBS
    = { { "nop", NOP }, { "ret", RET }, { "jmp", JMP }, { "run", RUN },
        { "sys", SYS }, { "wdg", WDG }, { "cmp", CMP }, { "jc", JC },
        { "jnc", JNC }, { "ent", ENT }, { "uic", UIC }, { "uip", UIP },
        { "ln", LN },   { "psh", PSH }, { "pop", POP }, { "mov", MOV },
        { "pp", PP },   { "cr", CR } };

static unsigned int
getVerb(const std::string &v)
{
  if(VERBS.contains(v))
    {
      return VERBS[v];
    }
  return 0;
}

static std::string
preprocessProgram(const std::string src)
{
  auto sv = Commons::splitStr(src, "\n");
  std::string srcp = src;

  for(auto &l : sv)
    {
      // Defines
      if(l.starts_with(".aka"))
        {
          std::string dname, dval, aka;
          std::stringstream ss(l);
          ss >> aka >> dname >> dval;
          if(dname.size() > 0 && dval.size() > 0)
            {
              std::regex repl(dname);
              while(srcp.contains(dname))
                {
                  srcp = std::regex_replace(srcp, repl, dval);
                }
            }
        }
    }
  return srcp;
}

Program::Program(const std::string &src, const std::string &n, DataPool *s)
{
  storage = s;
  name = n;
  frame.prog = this;
  auto lines = Commons::splitStr(preprocessProgram(src), "\n");
  unsigned int i = 0;
  for(auto s : lines)
    {
      while(s.starts_with(' '))
        {
          s = s.substr(1);
        }
      if(s.size() == 0)
        {
          continue;
        }
      if(s.starts_with(";") || s.starts_with("."))
        {
          continue;
        }
      if(s.contains(';'))
        {
          s = Commons::splitStr(s, ";")[0];
        }
      std::stringstream ss(s);
      std::string verb, a1, a2;
      ss >> verb >> a1 >> a2;
      if(verb.size() > 0)
        {
          if(verb.starts_with("::"))
            {
              labels[verb.substr(2)] = i;
            }
          else
            {
              Instr ins;
              ins.a1 = a1;
              ins.a2 = a2;
              ins.verb = getVerb(verb);
              bin.push_back(ins);
              i++;
            }
        }
    }
}

std::string
Program::resolveValue(const std::string &label)
{
  if(label.starts_with("%"))
    {
      return locals[label];
    }
  else if(label.starts_with("$"))
    {
      // In storage area
      // If label starts with $$ it will be automatically stored to file
      return (*storage)[label];
    }
  else
    {
      return label; // As is
    }
}

std::string &
Program::resolveValueRef(const std::string &label)
{
  if(label.starts_with("%"))
    {
      return locals[label];
    }
  else if(label.starts_with("$"))
    {
      // In storage area
      // If label starts with $$ it will be automatically stored to file
      return (*storage)[label];
    }
  else
    {
      // Fallback, actually nothing
      return locals[label];
    }
}

void
Program::run(PCallback cb)
{
  bool running = true;
  while(running)
    {
      if(eip >= bin.size())
        {
          break;
        }
      else
        {
          auto curInstr = bin[eip];
          switch(curInstr.verb)
            {
            case NOP:
            default:
              eip++;
              break;
            case RET:
              running = false;
              cb();
              break;
            case JMP:
              if(!labels.contains(curInstr.a1))
                {
                  LOG("Warning: Invalid jump target " << curInstr.a1);
                  eip++;
                }
              else
                {
                  eip = labels[curInstr.a1];
                }
              break;
            case RUN:
              eip++;
              running = false;
              runProgram(
                  curInstr.a1, [cb, this]() -> void { this->run(cb); },
                  storage);
              break;
            case SYS:
              eip++;
              if(SYSCALLCTL.contains(curInstr.a1))
                {
                  running = false;
                  auto foo = SYSCALLCTL[curInstr.a1];
                  foo(*this, [cb, this]() -> void { this->run(cb); });
                }
              else
                {
                  LOG("Warning: Invalid system call " << curInstr.a1);
                }
              break;
            case WDG:
              frame.mkWidget(curInstr.a1);
              eip++;
              break;
            case CMP:
              if(resolveValue(curInstr.a1) == resolveValue(curInstr.a2))
                {
                  carry = true;
                }
              else
                {
                  carry = false;
                }
              eip++;
              break;
            case JC:
              if(carry)
                {
                  eip = labels[curInstr.a1];
                }
              else
                {
                  eip++;
                }
              break;
            case JNC:
              if(!carry)
                {
                  eip = labels[curInstr.a1];
                }
              else
                {
                  eip++;
                }
              break;
            case ENT:
              frame.mkEntry(curInstr.a1);
              eip++;
              break;
            case UIC:
              eip++;
              running = false;
              frame.run([cb, this]() -> void { this->run(cb); });
              break;
            case UIP:
              eip++;
              frame.run(nullptr);
              break;
            case LN:
              frame.linkLabel = curInstr.a1; // Do not resolve
              eip++;
              break;
            case PSH:
              stack.push_back(resolveValue(curInstr.a1));
              eip++;
              break;
            case POP:
              if(stack.size() == 0)
                {
                  LOG("Warning: Stack overflow detected in " << name);
                }
              else
                {
                  resolveValueRef(curInstr.a1) = std::string(stack.back());
                  stack.pop_back();
                }
              eip++;
              break;
            case MOV:
              resolveValueRef(curInstr.a2) = resolveValue(curInstr.a1);
              eip++;
              break;
            case PP:
              frame.props[resolveValue(curInstr.a1)]
                  = resolveValue(curInstr.a2);
              eip++;
              break;
            case CR:
              if(resolveValue(curInstr.a1) == "0")
                {
                  carry = false;
                }
              else
                {
                  carry = true;
                }
              eip++;
              break;
            }
        }
    }
}

void
runProgram(const std::string &name, PCallback cb, DataPool *d)
{
  auto fp = resolveName(name);
  std::ifstream f(fp);
  if(f.fail())
    {
      cb();
    }
  else
    {
      std::stringstream ss;
      ss << f.rdbuf();
      auto src = ss.str();
      Program *pg = new Program(src, name, d);
      pg->run([cb, pg]() -> void {
        delete pg;
        cb();
      });
    }
}
void
bindSysCall(const std::string &name, PSysCall fun)
{
  SYSCALLCTL[name] = fun;
}

}
}