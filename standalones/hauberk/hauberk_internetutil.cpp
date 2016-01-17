// hauberk_internetutil.cpp

#include <hauberk_internetutil.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                             // ------------------
                             // class InternetUtil
                             // ------------------

// CLASS METHODS
std::uint8_t InternetUtil::version(const std::uint8_t *buffer)
{
    return BufferUtil::slice(buffer[0], 0, 4);
}

std::uint8_t InternetUtil::headerLength(const std::uint8_t *buffer)
{
    return 4 * BufferUtil::slice(buffer[0], 4, 8);
}

std::uint8_t InternetUtil::differentiatedServicesCodePoint(
                                                    const std::uint8_t *buffer)
{
    return BufferUtil::slice(buffer[1], 0, 6);
}

std::uint8_t InternetUtil::explicitCongestioNotification(
                                                    const std::uint8_t *buffer)
{
    return BufferUtil::slice(buffer[1], 6, 8);
}

std::uint16_t InternetUtil::length(const std::uint8_t *buffer)
{
    std::uint16_t length;
    BufferUtil::copy(&length, buffer + 2);
    BufferUtil::toHostOrder(&length);
    return length;
}

std::uint16_t InternetUtil::identification(const std::uint8_t *buffer)
{
    std::uint16_t identification;
    BufferUtil::copy(&identification, buffer + 4);
    BufferUtil::toHostOrder(&identification);
    return identification;
}

std::uint8_t InternetUtil::flags(const std::uint8_t *buffer)
{
    std::uint16_t sixthSeventhBytes;
    BufferUtil::copy(&sixthSeventhBytes, buffer + 6);
    return BufferUtil::slice(sixthSeventhBytes, 0, 3);
}

std::uint16_t InternetUtil::fragmentOffset(const std::uint8_t *buffer)
{
    std::uint16_t sixthSeventhBytes;
    BufferUtil::copy(&sixthSeventhBytes, buffer + 6);
    return BufferUtil::slice(sixthSeventhBytes, 3, 16);
}

std::uint8_t InternetUtil::timeToLive(const std::uint8_t *buffer)
{
    return buffer[8];
}

std::uint8_t InternetUtil::protocol(const std::uint8_t *buffer)
{
    return buffer[9];
}

std::uint16_t InternetUtil::headerChecksum(const std::uint8_t *buffer)
{
    std::uint16_t headerChecksum;
    BufferUtil::copy(&headerChecksum, buffer + 10);
    return headerChecksum;
}

void InternetUtil::setSourceAddress(std::uint8_t  *buffer,
                                    std::uint32_t  address)
{
    BufferUtil::copy(buffer + 12, address);
}

std::uint32_t InternetUtil::sourceAddress(const std::uint8_t *buffer)
{
    std::uint32_t sourceAddress;
    BufferUtil::copy(&sourceAddress, buffer + 12);
    return sourceAddress;
}

void InternetUtil::setDestinationAddress(std::uint8_t  *buffer,
                                         std::uint32_t  address)
{
    BufferUtil::copy(buffer + 16, address);
}

std::uint32_t InternetUtil::destinationAddress(const std::uint8_t *buffer)
{
    std::uint32_t destinationAddress;
    BufferUtil::copy(&destinationAddress, buffer + 16);
    return destinationAddress;
}

std::uint8_t InternetUtil::optionsLength(const std::uint8_t *buffer)
{
    return headerLength(buffer) - 20;
}

const std::uint8_t *InternetUtil::optionsData(const std::uint8_t *buffer)
{
    return buffer + 20;
}

std::uint8_t *InternetUtil::payload(std::uint8_t *buffer)
{
    return buffer + headerLength(buffer);
}

const std::uint8_t *InternetUtil::payload(const std::uint8_t *buffer)
{
    return buffer + headerLength(buffer);
}

}  // close namespace hauberk


