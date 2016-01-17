// hauberk_loopbackutil.cpp

#include <hauberk_loopbackutil.h>

#include <hauberk_bufferutil.h>

#include <algorithm>

namespace hauberk {

                             // ------------------
                             // class LoopbackUtil
                             // ------------------

// CLASS METHODS
void LoopbackUtil::setProtocolFamily(std::uint8_t *buffer, Family family)
{
    BufferUtil::copy(buffer, family);
}

std::uint32_t LoopbackUtil::protocolFamily(const std::uint8_t *buffer)
{
    std::uint32_t result;
    BufferUtil::copy(&result, buffer);
    return result;
}

void LoopbackUtil::copyPayload(std::uint8_t       *buffer,
                               const std::uint8_t *payloadData,
                               std::size_t         payloadLength)
{
    std::copy(payloadData, payloadData + payloadLength, buffer + 4);
}

const std::uint8_t *LoopbackUtil::payload(const std::uint8_t *buffer)
{
    return buffer + 4;
}

}  // close namespace hauberk

