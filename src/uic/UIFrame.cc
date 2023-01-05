#include "ach/uic/UIFrame.hh"

#include <log.hh>
#include "ach/uic/Brain.hh"
#include "ach/uic/Protocol.hh"
#include <cJSON.h>

namespace Alicorn
{
namespace UIC
{

void
UIFrame::run(PCallback cb, bool wait)
{
  auto wgs = cJSON_CreateArray();
  for(auto &w : widgets)
    {
      auto o = cJSON_CreateObject();
      cJSON_AddStringToObject(o, "variant", w.first.c_str());
      auto pp = cJSON_CreateObject();
      for(auto &p : w.second)
        {
          cJSON_AddStringToObject(pp, p.first.c_str(), p.second.c_str());
        }
      cJSON_AddItemToObject(o, "props", pp);
      cJSON_AddItemToArray(wgs, o);
    }
  auto ets = cJSON_CreateArray();
  for(auto &e : entries)
    {
      auto o = cJSON_CreateObject();
      cJSON_AddStringToObject(o, "variant", e.variant.c_str());
      cJSON_AddStringToObject(o, "jmpLabel", e.jmpLabel.c_str());
      auto pp = cJSON_CreateObject();
      for(auto &p : e.props)
        {
          cJSON_AddStringToObject(pp, p.first.c_str(), p.second.c_str());
        }
      cJSON_AddItemToObject(o, "props", pp);
      cJSON_AddItemToArray(ets, o);
    }
  auto obj = cJSON_CreateObject();
  cJSON_AddItemToObject(obj, "widgets", wgs);
  cJSON_AddItemToObject(obj, "entries", ets);
  widgets.clear();
  entries.clear();

  if(wait)
    {
      UIC::bindListener(
          "UIResponse",
          [cb, this](const std::string &data, Callback fcb) -> void {
            fcb("");
            auto res = cJSON_Parse(data.c_str());
            auto userChoice = cJSON_GetObjectItem(res, "userChoice");
            if(cJSON_IsString(userChoice))
              {
                std::string jl = cJSON_GetStringValue(userChoice);
                if(this->prog->labels.contains(jl))
                  {
                    // Do jump if contains, else will just continue from 'uic'
                    this->prog->eip = this->prog->labels[jl];
                  }

                cb();

                return;
              }
            auto userInput = cJSON_GetObjectItem(res, "userInput");
            if(cJSON_IsArray(userInput))
              {
                auto sz = cJSON_GetArraySize(userInput);
                for(int i = 0; i < sz; i++)
                  {
                    auto cur = cJSON_GetArrayItem(userInput, i);
                    this->prog->stack.push_back(cJSON_GetStringValue(cur));
                  }
                cb();
                return;
              }
          },
          true);
    }
  else
    {
      UIC::bindListener(
          "UIDrew",
          [cb](const std::string &data, Callback fcb) -> void {
            fcb("");
            cb();
          },
          true);
    }
  UIC::sendMessage("UIDraw", cJSON_Print(obj));
  cJSON_Delete(obj);
}
void
UIFrame::mkWidget(const std::string &var)
{
  Widget wg;
  wg.first = var;
  wg.second = props;
  widgets.push_back(wg);
}

void
UIFrame::mkEntry(const std::string &var)
{
  Entry ent;
  ent.jmpLabel = linkLabel;
  ent.props = props;
  ent.variant = var;
  entries.push_back(ent);
}

}
}