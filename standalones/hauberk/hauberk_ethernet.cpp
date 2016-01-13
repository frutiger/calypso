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
const Ethernet::Address Ethernet::destinationAddress() const
{
    Address address;
    BufferUtil::copy(&address, d_buffer[0]);
    return address;
}

const Ethernet::Address Ethernet::sourceAddress() const
{
    Address address;
    BufferUtil::copy(&address, d_buffer[6]);
    return address;
}

Ethernet::Type Ethernet::type() const
{
    Type type;
    BufferUtil::copy(&type, d_buffer[12]);
    return type;
}

const std::uint8_t *Ethernet::payload() const
{
    return d_buffer + 14;
}

}  // close namespace hauberk

