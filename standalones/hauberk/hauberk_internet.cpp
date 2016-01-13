// hauberk_internet.cpp

#include <hauberk_internet.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

// CREATORS
Internet::Internet(const std::uint8_t *buffer)
: d_buffer(buffer)
{
}

// ACCESSORS
const std::uint8_t *Internet::buffer() const
{
    return d_buffer;
}

std::uint8_t Internet::version() const
{
    std::uint8_t firstByte;
    BufferUtil::copy(&firstByte, *d_buffer);
    return BufferUtil::slice(firstByte, 0, 4);
}

std::uint8_t Internet::headerLength() const
{
    std::uint8_t firstByte;
    BufferUtil::copy(&firstByte, d_buffer[0]);
    return 4 * BufferUtil::slice(firstByte, 4, 8);
}

std::uint8_t Internet::differentiatedServicesCodePoint() const
{
    std::uint8_t secondByte;
    BufferUtil::copy(&secondByte, d_buffer[1]);
    return BufferUtil::slice(secondByte, 0, 6);
}

std::uint8_t Internet::explicitCongestioNotification() const
{
    std::uint8_t secondByte;
    BufferUtil::copy(&secondByte, d_buffer[1]);
    return BufferUtil::slice(secondByte, 6, 8);
}

std::uint16_t Internet::length() const
{
    std::uint16_t length;
    BufferUtil::copy(&length, d_buffer[2]);
    BufferUtil::toHostOrder(&length);
    return length;
}

std::uint16_t Internet::identification() const
{
    std::uint16_t identification;
    BufferUtil::copy(&identification, d_buffer[4]);
    BufferUtil::toHostOrder(&identification);
    return identification;
}

std::uint8_t Internet::flags() const
{
    std::uint16_t sixthSeventhBytes;
    BufferUtil::copy(&sixthSeventhBytes, d_buffer[6]);
    return BufferUtil::slice(sixthSeventhBytes, 0, 3);
}

std::uint16_t Internet::fragmentOffset() const
{
    std::uint16_t sixthSeventhBytes;
    BufferUtil::copy(&sixthSeventhBytes, d_buffer[6]);
    return BufferUtil::slice(sixthSeventhBytes, 3, 16);
}

std::uint8_t Internet::timeToLive() const
{
    std::uint8_t timeToLive;
    BufferUtil::copy(&timeToLive, d_buffer[8]);
    return timeToLive;
}

std::uint8_t Internet::protocol() const
{
    std::uint8_t protocol;
    BufferUtil::copy(&protocol, d_buffer[9]);
    return protocol;
}

std::uint16_t Internet::headerChecksum() const
{
    std::uint16_t headerChecksum;
    BufferUtil::copy(&headerChecksum, d_buffer[10]);
    return headerChecksum;
}

std::uint32_t Internet::sourceAddress() const
{
    std::uint32_t sourceAddress;
    BufferUtil::copy(&sourceAddress, d_buffer[12]);
    return sourceAddress;
}

std::uint32_t Internet::destinationAddress() const
{
    std::uint32_t destinationAddress;
    BufferUtil::copy(&destinationAddress, d_buffer[16]);
    return destinationAddress;
}

std::uint8_t Internet::optionsLength() const
{
    return headerLength() - 20;
}

const std::uint8_t *Internet::optionsData() const
{
    return d_buffer + 20;
}

const std::uint8_t *Internet::payload() const
{
    return d_buffer + headerLength();
}

}  // close namespace hauberk


