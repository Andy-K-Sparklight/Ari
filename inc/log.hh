#ifndef INCLUDE_ALICORN_CH_LOG
#define INCLUDE_ALICORN_CH_LOG

#ifndef ACH_LOG_QUIET
#include <iostream>
#include <sstream>
#ifndef ACH_LOG_NOUI
#include "ach/uic/Protocol.hh"
#define sendToUI(text) Alicorn::UIC::sendMessage("AddLog", text);
#else
#define sendToUI(text)
#endif

#define LOG(text)                                                             \
  {                                                                           \
    std::stringstream s;                                                      \
    s << "[" __FILE__ << ":" << __LINE__ << "] " << text << "\n";             \
    std::cout << s.str();                                                     \
    sendToUI(s.str());                                                        \
  }
#else
#define LOG(test)
#endif

#endif /* INCLUDE_ALICORN_CH_LOG */
