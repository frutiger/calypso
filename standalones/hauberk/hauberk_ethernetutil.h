// hauberk_ethernetutil.h
#ifndef HAUBERK_ETHERNETUTIL
#define HAUBERK_ETHERNETUTIL

#ifndef INCLUDED_ARRAY
#define INCLUDED_ARRAY
#include <array>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                             // ==================
                             // class EthernetUtil
                             // ==================

struct EthernetUtil {
    // TYPES
    enum class Type : std::uint16_t {
        INTERNET = 0x0800,
        ARP      = 0x0806,
    };

    typedef std::array<std::uint8_t, 6> Address;

    // CLASS METHODS
    static void setDestinationAddress(std::uint8_t   *buffer,
                                      const Address&  address);
        // TBD: contract

    static void destinationAddress(Address            *address,
                                   const std::uint8_t *buffer);
        // TBD: contract

    static void setSourceAddress(std::uint8_t *buffer, const Address& address);
        // TBD: contract

    static void sourceAddress(Address *address, const std::uint8_t *buffer);
        // TBD: contract

    static void setType(std::uint8_t *buffer, Type type);
        // TBD: contract

    static Type type(const std::uint8_t *buffer);
        // TBD: contract

    static void copyPayload(std::uint8_t       *buffer,
                            const std::uint8_t *payload,
                            std::size_t         payloadLength);
        // TBD: contract

    static const std::uint8_t *payload(const std::uint8_t *buffer);
        // TBD: contract

    static std::uint32_t frameCheckSequence(const std::uint8_t *buffer);
        // TBD: contract
};

}  // close namespace hauberk

#endif
