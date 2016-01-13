// hauberk_loopback.h
#ifndef HAUBERK_LOOPBACK
#define HAUBERK_LOOPBACK

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

class Loopback {
    // DATA
    const uint8_t *d_buffer;

  public:
    // CREATORS
    explicit Loopback(const uint8_t *buffer);

    // ACCESSORS
    std::uint32_t protocolFamily() const;
        // TBD: contract

    const uint8_t *rest() const;
        // TBD: contract
};

}  // close namespace hauberk

#endif
