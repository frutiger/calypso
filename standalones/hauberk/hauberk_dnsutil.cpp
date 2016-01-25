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
    BufferUtil::toNetworkOrder(&transactionId);
    BufferUtil::copy(buffer, transactionId);
}

std::uint16_t DnsUtil::transactionId(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::flags(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer + 2);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::numQueries(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer + 4);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::numResponses(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer + 6);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::numAuthorities(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer + 8);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::numAdditional(const std::uint8_t *buffer)
{
    std::uint16_t result;
    BufferUtil::copy(&result, buffer + 10);
    BufferUtil::toHostOrder(&result);
    return result;
}

const std::uint8_t *DnsUtil::findRecord(const std::uint8_t *buffer,
                                        std::uint32_t       recordNumber)
{
    const std::uint8_t *record = buffer + 12;
    while (recordNumber != 0) {
        while (*record != '\0') {
            ++record;
        }
        record += 4;
        --recordNumber;
    }
    return record;
}

std::uint32_t DnsUtil::recordLabelLength(const std::uint8_t *record)
{
    std::uint8_t length = 0;
    std::size_t  offset = 0;
    while (true) {
        if (record[offset] == '\0') {
            return length;
        }
        length += record[offset];
        offset += record[offset] + 1;
    }
}

std::uint16_t DnsUtil::recordType(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + recordLabelLength(record) + 1);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::recordClass(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + 2 + recordLabelLength(record) + 1);
    BufferUtil::toHostOrder(&result);
    return result;
}

}  // close namespace 'hauberk'

