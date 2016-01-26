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

std::uint32_t DnsUtil::recordLabelLength(const std::uint8_t *record)
{
    const std::uint8_t *offset = record;
    if (*offset & 0xc0) {
        return 2;
    }
    while (*offset != '\0') {
        offset += 1 + *offset;
    }
    return (offset - record) + 1;
}

const std::uint8_t *DnsUtil::findRecord(const std::uint8_t *buffer,
                                        std::uint32_t       recordNumber)
{
    const std::uint8_t *record = buffer + 12;
    while (recordNumber != 0) {
        record += recordLabelLength(record) + 4;
        --recordNumber;
    }
    return record;
}

std::uint16_t DnsUtil::recordType(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + recordLabelLength(record));
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::recordClass(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + 2 + recordLabelLength(record));
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint32_t DnsUtil::recordTimeToLive(const std::uint8_t *record)
{
    std::uint32_t result;
    BufferUtil::copy(&result, record + 4 + recordLabelLength(record));
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::recordDataLength(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + 8 + recordLabelLength(record));
    BufferUtil::toHostOrder(&result);
    return result;
}

const std::uint8_t *DnsUtil::recordData(const std::uint8_t *record)
{
    return record + 10 + recordLabelLength(record);
}

}  // close namespace 'hauberk'

