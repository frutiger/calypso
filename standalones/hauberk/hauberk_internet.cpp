// hauberk_internet.cpp

#include <hauberk_internet.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

// CREATORS
Internet::Internet(const uint8_t *buffer)
: d_buffer(buffer)
{
}

// ACCESSORS
std::uint8_t Internet::version() const
{
    std::uint8_t firstByte;
    BufferUtil::copy(&firstByte, *d_buffer);
    return BufferUtil::slice(firstByte, 4, 8);
}

const uint8_t *Internet::rest() const
{
    return d_buffer + 4;
}

}  // close namespace hauberk


