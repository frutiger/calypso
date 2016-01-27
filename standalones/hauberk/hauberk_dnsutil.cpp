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

std::uint32_t DnsUtil::totalRecordLabelLength(const std::uint8_t *record)
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
        record += totalRecordLabelLength(record) + 4;
        --recordNumber;
    }
    return record;
}

std::uint8_t DnsUtil::recordLabelLength(const std::uint8_t *buffer,
                                        const std::uint8_t *record)
{
    if (*record & 0xc0) {
        return buffer[*(record + 1)];
    }
    else {
        return *record;
    }
}

const std::uint8_t *DnsUtil::recordLabel(const std::uint8_t *buffer,
                                         const std::uint8_t *record)
{
    if (*record & 0xc0) {
        return buffer + *(record + 1) + 1;
    }
    else {
        return record + 1;
    }
}

void DnsUtil::walkLabels(const std::uint8_t  *buffer,
                         const std::uint8_t  *record,
                         const LabelHandler&  labelHandler)
{
    const std::uint8_t *labelOffset = record;
    while (true) {
        std::uint8_t labelLength = recordLabelLength(buffer, labelOffset);
                     labelOffset = recordLabel(buffer, labelOffset);
        if (labelLength == 0) {
            return;                                                   // RETURN
        }

        labelHandler(labelOffset, labelLength);
        labelOffset += labelLength;
    }
}

std::uint16_t DnsUtil::recordType(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + totalRecordLabelLength(record));
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::recordClass(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + totalRecordLabelLength(record) + 2);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint32_t DnsUtil::recordTimeToLive(const std::uint8_t *record)
{
    std::uint32_t result;
    BufferUtil::copy(&result, record + totalRecordLabelLength(record) + 4);
    BufferUtil::toHostOrder(&result);
    return result;
}

std::uint16_t DnsUtil::recordDataLength(const std::uint8_t *record)
{
    std::uint16_t result;
    BufferUtil::copy(&result, record + totalRecordLabelLength(record) + 8);
    BufferUtil::toHostOrder(&result);
    return result;
}

const std::uint8_t *DnsUtil::recordData(const std::uint8_t *record)
{
    return record + totalRecordLabelLength(record) + 10;
}

}  // close namespace 'hauberk'

