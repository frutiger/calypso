// hauberk_udp.cpp

#include <hauberk_udp.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                                 // ---------
                                 // class Udp
                                 // ---------

// CREATORS
Udp::Udp(const std::uint8_t *buffer)
: d_buffer(buffer)
{
}

// ACCESSORS
std::uint16_t Udp::sourcePort() const
{
    std::uint16_t sourcePort;
    BufferUtil::copy(&sourcePort, d_buffer[0]);
    BufferUtil::toHostOrder(&sourcePort);
    return sourcePort;
}

std::uint16_t Udp::destinationPort() const
{
    std::uint16_t destinationPort;
    BufferUtil::copy(&destinationPort, d_buffer[2]);
    BufferUtil::toHostOrder(&destinationPort);
    return destinationPort;
}

std::uint16_t Udp::length() const
{
    std::uint16_t length;
    BufferUtil::copy(&length, d_buffer[4]);
    BufferUtil::toHostOrder(&length);
    return length;
}

std::uint16_t Udp::checksum() const
{
    std::uint16_t checksum;
    BufferUtil::copy(&checksum, d_buffer[6]);
    BufferUtil::toHostOrder(&checksum);
    return checksum;
}

const std::uint8_t *Udp::rest() const
{
    return d_buffer + 8;
}

}  // close namespace 'hauberk'

