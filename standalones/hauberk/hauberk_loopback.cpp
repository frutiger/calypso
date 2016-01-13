// hauberk_loopback.cpp

#include <hauberk_loopback.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

// CREATORS
Loopback::Loopback(const std::uint8_t *buffer)
: d_buffer(buffer)
{
}

// ACCESSORS
std::uint32_t Loopback::protocolFamily() const
{
    std::uint32_t result;
    BufferUtil::copy(&result, *d_buffer);
    return result;
}

const std::uint8_t *Loopback::rest() const
{
    return d_buffer + 4;
}

}  // close namespace hauberk

