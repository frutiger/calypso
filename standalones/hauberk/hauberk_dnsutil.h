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
    // CLASS METHODS
    static void setTransactionId(std::uint8_t  *buffer,
                                 std::uint16_t  transactionId);
        // TBD: contract

    static std::uint16_t transactionId(const std::uint8_t *buffer);
        // TBD: contract

    // TBD: add remaining accessors
};

}  // close namespace 'hauberk'

#endif
