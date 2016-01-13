// hauberk_ethernet.h
#ifndef HAUBERK_ETHERNET
#define HAUBERK_ETHERNET

#ifndef INCLUDED_ARRAY
#define INCLUDED_ARRAY
#include <array>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                               // ==============
                               // class Ethernet
                               // ==============

class Ethernet {
    // DATA
    const std::uint8_t *d_buffer;

  public:
    // TYPES
    typedef std::array<std::uint8_t, 6> Address;

    // CREATORS
    explicit Ethernet(const std::uint8_t *buffer);

    // ACCESSORS
    const Address destinationAddress() const;
        // TBD: contract

    const Address sourceAddress() const;
        // TBD: contract

    std::uint16_t packetLength() const;
        // TBD: contract

    const std::uint8_t *payload() const;
        // TBD: contract

    std::uint32_t frameCheckSequence() const;
        // TBD: contract

};

}  // close namespace hauberk

#endif
