// hauberk_dns.cpp

#include <hauberk_dns.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                                 // ---------
                                 // class Dns
                                 // ---------

// CREATORS
Dns::Dns(const std::uint8_t *buffer)
: d_buffer(buffer)
{
}

// ACCESSORS
std::uint16_t Dns::transactionId() const
{
    std::uint16_t result;
    BufferUtil::slice(&result, d_buffer[0]);
    BufferUtil::toHostOrder(&result);
    return result;
}

}  // close namespace 'hauberk'

