// hauberk_dnsutil.h
#ifndef HAUBERK_DNSUTIL
#define HAUBERK_DNSUTIL

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                               // =============
                               // class DnsUtil
                               // =============

struct DnsUtil {
    // TYPES
    enum class Type : std::uint16_t {
        A    = 1,
        AAAA = 28,
    };

    enum class Class : std::uint16_t {
        INTERNET = 1,
    };

    // CLASS METHODS
    static void setTransactionId(std::uint8_t  *buffer,
                                 std::uint16_t  transactionId);
        // TBD: contract

    static std::uint16_t transactionId(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t flags(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t numQueries(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t numResponses(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t numAuthorities(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t numAdditional(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint32_t recordLabelLength(const std::uint8_t *record);
        // TBD: contract

    static const std::uint8_t *findRecord(const std::uint8_t *buffer,
                                          std::uint32_t       recordNumber);
        // TBD: contract

    static std::uint16_t recordType(const std::uint8_t *record);
        // TBD: contract

    static std::uint16_t recordClass(const std::uint8_t *record);
        // TBD: contract

    static std::uint32_t recordTimeToLive(const std::uint8_t *record);
        // TBD: contract

    static std::uint16_t recordDataLength(const std::uint8_t *record);
        // TBD: contract

    static const std::uint8_t *recordData(const std::uint8_t *record);
        // TBD: contract

    // TBD: add remaining accessors
};

}  // close namespace 'hauberk'

#endif
