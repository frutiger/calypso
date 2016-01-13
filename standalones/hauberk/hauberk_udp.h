// hauberk_udp.h
#ifndef HAUBERK_UDP
#define HAUBERK_UDP

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                                 // =========
                                 // class Udp
                                 // =========

class Udp {
    // DATA
    const uint8_t *d_buffer;

  public:
    // TYPES
    enum class Port : std::uint16_t {
        DNS = 53,
    };

    // CREATORS
    explicit Udp(const uint8_t *buffer);
        // TBD: contract

    // ACCESSORS
    std::uint16_t sourcePort() const;
        // TBD: contract

    std::uint16_t destinationPort() const;
        // TBD: contract

    std::uint16_t length() const;
        // TBD: contract

    std::uint16_t checksum() const;
        // TBD: contract

    const std::uint8_t *rest() const;
        // TBD: contract
};

}  // close namespace 'hauberk'

#endif
