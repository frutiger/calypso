// hauberk_ethernet.cpp

#include <hauberk_ethernet.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                               // --------------
                               // class Ethernet
                               // --------------

// CREATORS
Ethernet::Ethernet(const std::uint8_t *buffer)
: d_buffer(buffer)
{
}

// ACCESSORS
const std::uint8_t *Ethernet::payload() const
{
    return d_buffer;
}

}  // close namespace hauberk

