#ifndef INCLUDE_LOG
#define INCLUDE_LOG

#ifndef ACH_LOG_QUIET
#include <iostream>
#define LOG(text)                                                             \
  std::cout << "[" __FILE__ << ":" << __LINE__ << "] " << text << "\n";
#else
#define LOG(test)
#endif

#endif /* INCLUDE_LOG */
