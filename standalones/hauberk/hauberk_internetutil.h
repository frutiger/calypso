// hauberk_internetutil.h
#ifndef HAUBERK_INTERNETUTIL
#define HAUBERK_INTERNETUTIL

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                             // ==================
                             // class InternetUtil
                             // ==================

struct InternetUtil {
    // TYPES
    enum class Protocol : std::uint8_t {
        ICMP =  1,
        TCP  =  6,
        UDP  = 17,
    };

    // CLASS METHODS
    static std::uint8_t version(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t headerLength(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t differentiatedServicesCodePoint(
                                                   const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t explicitCongestioNotification(
                                                   const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t length(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t identification(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t flags(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint16_t fragmentOffset(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t timeToLive(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t protocol(const std::uint8_t *buffer);
        // TBD: contract

    static void updateChecksum(std::uint8_t *buffer);
        // TBD: contract

    static void setSourceAddress(std::uint8_t *buffer, std::uint32_t address);
        // TBD: contract

    static std::uint32_t sourceAddress(const std::uint8_t *buffer);
        // TBD: contract

    static void setDestinationAddress(std::uint8_t  *buffer,
                                      std::uint32_t  address);
        // TBD: contract

    static std::uint32_t destinationAddress(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t optionsLength(const std::uint8_t *buffer);
        // TBD: contract

    static const std::uint8_t *optionsData(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint8_t *payload(std::uint8_t *buffer);
        // TBD: contract

    static const std::uint8_t *payload(const std::uint8_t *buffer);
        // TBD: contract
};

}  // close namespace 'hauberk'

#endif
