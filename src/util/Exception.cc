#include "ach/util/Exception.hh"

namespace Alicorn
{
namespace Exception
{

ExternalException::ExternalException(const std::string &verb, const char *ext)
    : reason("EXTERNAL: " + verb + ext)
{
}

ExternalException::ExternalException(const std::string &verb,
                                     const std::string &ext)
    : reason("EXTERNAL: " + verb + ext)
{
}

const char *
ExternalException::what() noexcept
{
  return reason.c_str();
}

InternalException::InternalException(const std::string &verb, const char *ext)
    : reason("INTERNAL: " + verb + ext)
{
}

InternalException::InternalException(const std::string &verb,
                                     const std::string &ext)
    : reason("INTERNAL: " + verb + ext)
{
}

const char *
InternalException::what() noexcept
{
  return reason.c_str();
}

};
}