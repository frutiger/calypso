// hauberk_ethernetutil.cpp

#include <hauberk_ethernetutil.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                             // ------------------
                             // class EthernetUtil
                             // ------------------

// CLASS METHODS
void EthernetUtil::setDestinationAddress(std::uint8_t   *buffer,
                                         const Address&  address)
{
    std::copy(address.begin(), address.end(), buffer);
}

void
EthernetUtil::destinationAddress(Address *address, const std::uint8_t *buffer)
{
    std::copy(buffer, buffer + 6, address->begin());
}

void EthernetUtil::setSourceAddress(std::uint8_t *buffer,
                                    const Address& address)
{
    std::copy(address.begin(), address.end(), buffer + 6);
}

void EthernetUtil::sourceAddress(Address *address, const std::uint8_t *buffer)
{
    std::copy(buffer, buffer + 12, address->begin());
}

void EthernetUtil::setType(std::uint8_t *buffer, Type type)
{
    std::uint16_t typeValue = static_cast<std::uint16_t>(type);
    BufferUtil::toNetworkOrder(&typeValue);
    BufferUtil::copy(buffer + 12, typeValue);
}

EthernetUtil::Type EthernetUtil::type(const std::uint8_t *buffer)
{
    std::uint16_t typeValue;
    BufferUtil::copy(&typeValue, buffer + 12);
    BufferUtil::toHostOrder(&typeValue);
    return static_cast<Type>(typeValue);
}

void EthernetUtil::copyPayload(std::uint8_t       *buffer,
                               const std::uint8_t *payload,
                               std::size_t         payloadLength)
{
    std::copy(payload, payload + payloadLength, buffer + 14);
}

std::uint8_t *EthernetUtil::payload(std::uint8_t *buffer)
{
    return buffer + 14;
}

const std::uint8_t *EthernetUtil::payload(const std::uint8_t *buffer)
{
    return buffer + 14;
}

}  // close namespace hauberk

