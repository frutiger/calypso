// hauberk_dnsutil.cpp

#include <hauberk_dnsutil.h>

#include <hauberk_bufferutil.h>

namespace hauberk {

                                 // ---------
                                 // class DnsUtil
                                 // ---------

// CLASS METHODS
void DnsUtil::setTransactionId(std::uint8_t  *buffer,
                               std::uint16_t  transactionId)
{
    BufferUtil::copy(buffer, transactionId);
}

std::uint16_t DnsUtil::transactionId(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer);
    BufferUtil::toHostOrder(&result);
    return result;
}

}  // close namespace 'hauberk'

