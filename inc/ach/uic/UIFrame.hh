#ifndef INCLUDE_ALICORN_CH_ACH_UIC_UIFRAME
#define INCLUDE_ALICORN_CH_ACH_UIC_UIFRAME

#include <string>
#include <vector>
#include <map>
#include <functional>

typedef std::map<std::string, std::string> UIProps;
typedef std::pair<std::string, UIProps> Widget;
typedef std::function<void()> PCallback;
typedef std::function<void(bool)> PTCallback;

namespace Alicorn
{
namespace UIC
{

typedef struct
{
  std::string variant;
  UIProps props;
  std::string jmpLabel;
} Entry;

class Program;
class UIFrame
{
public:
  UIProps props;
  std::string linkLabel;
  std::vector<Widget> widgets;
  std::vector<Entry> entries;
  Program *prog;

  void run(PCallback cb, bool wait = true);
  void mkWidget(const std::string &var);
  void mkEntry(const std::string &var);
};

}
}

#endif /* INCLUDE_ALICORN_CH_ACH_UIC_UIFRAME */
