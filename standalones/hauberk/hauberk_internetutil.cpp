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

void InternetUtil::updateChecksum(std::uint8_t *buffer)
{
    // zero out checksum
    BufferUtil::copy(buffer + 10, static_cast<std::uint16_t>(0));

    std::uint32_t accumulator = 0;

    // iterate over 16-bit words in header
    for (std::uint8_t i = 0; i < headerLength(buffer); i += 2) {
        // obtain i'th 16-bit word in network order
        std::uint16_t word;
        BufferUtil::copy(&word, buffer + i);
        BufferUtil::toNetworkOrder(&word);

        // accumulate word
        accumulator += word;
    }

    // perform 16-bit carry
    accumulator = (accumulator & 0xFFFF) + (accumulator >> 16);

    // complement of 16-bits
    std::uint16_t sum = accumulator;
    sum = ~sum;

    // back to network order
    BufferUtil::toNetworkOrder(&sum);

    BufferUtil::copy(buffer + 10, sum);
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


