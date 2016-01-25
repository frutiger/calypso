// hauberk_udputil.h
#ifndef HAUBERK_UDPUTIL
#define HAUBERK_UDPUTIL

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                               // =============
                               // class UdpUtil
                               // =============

struct UdpUtil {
    // TYPES
    enum class Port : std::uint16_t {
        DNS = 53,
    };

    // CLASS METHODS
    static std::uint16_t sourcePort(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t destinationPort(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t length(const std::uint8_t *buffer);
        // TBD: contract

    static void updateChecksum(std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t *payload(std::uint8_t *buffer);
        // TBD: contract

    static const std::uint8_t *payload(const std::uint8_t *buffer);
        // TBD: contract
};

}  // close namespace 'hauberk'

#endif
