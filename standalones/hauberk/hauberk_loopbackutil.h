// hauberk_loopbackutil.h
#ifndef HAUBERK_LOOPBACKUTIL
#define HAUBERK_LOOPBACKUTIL

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_CSTDLIB
#define INCLUDED_CSTDLIB
#include <cstdlib>
#endif

namespace hauberk {

                             // ==================
                             // class LoopbackUtil
                             // ==================

struct LoopbackUtil {
    // TYPES
    enum class Family : std::uint32_t {
        INTERNET = 2,
    };

    // CLASS METHODS
    static void setProtocolFamily(std::uint8_t *buffer, Family family);
        // TBD: contract

    static std::uint32_t protocolFamily(const std::uint8_t *buffer);
        // TBD: contract

    static void copyPayload(std::uint8_t       *buffer,
                            const std::uint8_t *payloadData,
                            std::size_t         payloadLength);
        // TBD: contract

    static const std::uint8_t *payload(const std::uint8_t *buffer);
        // TBD: contract
};

}  // close namespace hauberk

#endif
