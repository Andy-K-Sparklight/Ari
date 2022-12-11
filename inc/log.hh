#ifndef INCLUDE_LOG
#define INCLUDE_LOG

#include <iostream>
#define LOG(text)                                                             \
  std::cout << "[" __FILE__ << ":" << __LINE__ << "] " << text << "\n";

#endif /* INCLUDE_LOG */
