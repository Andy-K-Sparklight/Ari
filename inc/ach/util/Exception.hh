#ifndef INCLUDE_ALICORN_CH_ACH_UTIL_EXCEPTION
#define INCLUDE_ALICORN_CH_ACH_UTIL_EXCEPTION

#include <exception>
#include <string>

namespace Alicorn
{

namespace Exception
{

class ExternalException : std::exception
{
protected:
  std::string reason;

public:
  ExternalException(const std::string &verb, const char *ext);
  ExternalException(const std::string &verb, const std::string &ext);
  virtual const char *what() noexcept;
};

class InternalException : std::exception
{
protected:
  std::string reason;

public:
  InternalException(const std::string &verb, const char *ext);
  InternalException(const std::string &verb, const std::string &ext);
  virtual const char *what() noexcept;
};

}

}

#endif /* INCLUDE_ALICORN_CH_ACH_UTIL_EXCEPTION */
