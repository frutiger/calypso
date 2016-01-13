// hauberk_loopback.h
#ifndef HAUBERK_LOOPBACK
#define HAUBERK_LOOPBACK

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                               // ==============
                               // class Loopback
                               // ==============

class Loopback {
    // DATA
    const std::uint8_t *d_buffer;

  public:
    // CREATORS
    explicit Loopback(const std::uint8_t *buffer);

    // ACCESSORS
    std::uint32_t protocolFamily() const;
        // TBD: contract

    const std::uint8_t *payload() const;
        // TBD: contract
};

}  // close namespace hauberk

#endif
