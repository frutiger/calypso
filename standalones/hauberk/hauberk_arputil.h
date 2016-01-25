// hauberk_arputil.h
#ifndef HAUBERK_ARPUTIL
#define HAUBERK_ARPUTIL

#ifndef INCLUDED_ARRAY
#define INCLUDED_ARRAY
#include <array>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                               // =============
                               // class ArpUtil
                               // =============

struct ArpUtil {
    // TYPES
    typedef std::array<std::uint8_t, 6> Address;

    enum class HardwareType : std::uint16_t {
        ETHERNET = 1,
    };

    enum class ProtocolType : std::uint16_t {
        INTERNET = 0x0800,
    };

    enum class Operation : std::uint16_t {
        REQUEST = 1,
        REPLY   = 2,
    };

    // CLASS METHODS
    static void setHardwareType(std::uint8_t *buffer,
                                HardwareType  hardwareType);
        // TBD: contract

    static HardwareType hardwareType(const std::uint8_t *buffer);
        // TBD: contract

    static void setProtocolType(std::uint8_t  *buffer,
                                ProtocolType   protocolType);
        // TBD: contract

    static ProtocolType protocolType(const std::uint8_t *buffer);
        // TBD: contract

    static void setHardwareAddressLength(std::uint8_t *buffer,
                                         std::uint8_t  length);
        // TBD: contract

    static std::uint8_t hardwareAddressLength(const std::uint8_t *buffer);
        // TBD: contract

    static void setProtocolAddressLength(std::uint8_t *buffer,
                                         std::uint8_t  length);
        // TBD: contract

    static std::uint8_t protocolAddressLength(const std::uint8_t *buffer);
        // TBD: contract

    static void setOperation(std::uint8_t *buffer, Operation operation);
        // TBD: contract

    static Operation operation(const std::uint8_t *buffer);
        // TBD: contract

    static void setSenderHardwareAddress(std::uint8_t   *buffer,
                                         const Address&  address);
        // TBD: contract

    static Address senderHardwareAddress(const std::uint8_t *buffer);
        // TBD: contract

    static void setSenderProtocolAddress(std::uint8_t *buffer,
                                         std::uint32_t address);
        // TBD: contract

    static std::uint32_t senderProtocolAddress(const std::uint8_t *buffer);
        // TBD: contract

    static void setTargetHardwareAddress(std::uint8_t   *buffer,
                                         const Address&  address);
        // TBD: contract

    static Address targetHardwareAddress(const std::uint8_t *buffer);
        // TBD: contract

    static void setTargetProtocolAddress(std::uint8_t  *buffer,
                                         std::uint32_t  address);
        // TBD: contract

    static std::uint32_t targetProtocolAddress(const std::uint8_t *buffer);
        // TBD: contract
};

}  // close namespace 'hauberk'

#endif
