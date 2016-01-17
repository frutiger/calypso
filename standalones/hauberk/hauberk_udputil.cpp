// hauberk_udputil.cpp

#include <hauberk_udputil.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                               // -------------
                               // class UdpUtil
                               // -------------

// CLASS METHODS
std::uint16_t UdpUtil::sourcePort(const std::uint8_t *buffer)
{
    std::uint16_t sourcePort;
    BufferUtil::copy(&sourcePort, buffer);
    BufferUtil::toHostOrder(&sourcePort);
    return sourcePort;
}

std::uint16_t UdpUtil::destinationPort(const std::uint8_t *buffer)
{
    std::uint16_t destinationPort;
    BufferUtil::copy(&destinationPort, buffer + 2);
    BufferUtil::toHostOrder(&destinationPort);
    return destinationPort;
}

std::uint16_t UdpUtil::length(const std::uint8_t *buffer)
{
    std::uint16_t length;
    BufferUtil::copy(&length, buffer + 4);
    BufferUtil::toHostOrder(&length);
    return length;
}

std::uint16_t UdpUtil::checksum(const std::uint8_t *buffer)
{
    std::uint16_t checksum;
    BufferUtil::copy(&checksum, buffer + 6);
    BufferUtil::toHostOrder(&checksum);
    return checksum;
}

std::uint8_t *UdpUtil::payload(std::uint8_t *buffer)
{
    return buffer + 8;
}

const std::uint8_t *UdpUtil::payload(const std::uint8_t *buffer)
{
    return buffer + 8;
}

}  // close namespace 'hauberk'

