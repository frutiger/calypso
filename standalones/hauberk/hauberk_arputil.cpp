// hauberk_arputil.cpp

#include <hauberk_arputil.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                               // -------------
                               // class ArpUtil
                               // -------------

// CLASS METHODS
void ArpUtil::setHardwareType(std::uint8_t *buffer, HardwareType hardwareType)
{
    std::uint16_t type = static_cast<std::uint16_t>(hardwareType);
    BufferUtil::toNetworkOrder(&type);
    BufferUtil::copy(buffer, type);
}

void ArpUtil::setProtocolType(std::uint8_t *buffer, ProtocolType protocolType)
{
    std::uint16_t type = static_cast<std::uint16_t>(protocolType);
    BufferUtil::toNetworkOrder(&type);
    BufferUtil::copy(buffer + 2, type);
}

void ArpUtil::setHardwareAddressLength(std::uint8_t *buffer,
                                       std::uint8_t  length)
{
    BufferUtil::copy(buffer + 4, length);
}

void ArpUtil::setProtocolAddressLength(std::uint8_t *buffer,
                                       std::uint8_t  length)
{
    BufferUtil::copy(buffer + 5, length);
}

void ArpUtil::setOperation(std::uint8_t *buffer, Operation operation)
{
    std::uint16_t op = static_cast<std::uint16_t>(operation);
    BufferUtil::toNetworkOrder(&op);
    BufferUtil::copy(buffer + 6, op);
}

void ArpUtil::setSenderHardwareAddress(std::uint8_t   *buffer,
                                       const Address&  address)
{
    std::copy(address.begin(), address.end(), buffer + 8);
}

void ArpUtil::setSenderProtocolAddress(std::uint8_t  *buffer,
                                       std::uint32_t  address)
{
    BufferUtil::copy(buffer + 14, address);
}

void ArpUtil::setTargetHardwareAddress(std::uint8_t   *buffer,
                                       const Address&  address)
{
    std::copy(address.begin(), address.end(), buffer + 18);
}

void ArpUtil::setTargetProtocolAddress(std::uint8_t  *buffer,
                                       std::uint32_t  address)
{
    BufferUtil::copy(buffer + 24, address);
}

std::uint32_t ArpUtil::targetProtocolAddress(const std::uint8_t *buffer)
{
    std::uint32_t address;
    BufferUtil::copy(&address, buffer + 24);
    BufferUtil::toHostOrder(&address);
    return address;
}

}  // close namespace 'hauberk'

