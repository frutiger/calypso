// hauberk_internet.h
#ifndef HAUBERK_INTERNET
#define HAUBERK_INTERNET

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

class Internet {
    // DATA
    const uint8_t *d_buffer;

  public:
    // TYPES
    enum class Protocol : std::uint8_t {
        ICMP =  1,
        TCP  =  6,
        UDP  = 17,
    };

    // CREATORS
    explicit Internet(const uint8_t *buffer);
        // TBD: contract

    // ACCESSORS
    std::uint8_t version() const;
        // TBD: contract

    std::uint8_t headerLength() const;
        // TBD: contract

    std::uint8_t differentiatedServicesCodePoint() const;
        // TBD: contract

    std::uint8_t explicitCongestioNotification() const;
        // TBD: contract

    std::uint16_t length() const;
        // TBD: contract

    std::uint16_t identification() const;
        // TBD: contract

    std::uint8_t flags() const;
        // TBD: contract

    std::uint16_t fragmentOffset() const;
        // TBD: contract

    std::uint8_t timeToLive() const;
        // TBD: contract

    std::uint8_t protocol() const;
        // TBD: contract

    std::uint16_t headerChecksum() const;
        // TBD: contract

    std::uint32_t sourceAddress() const;
        // TBD: contract

    std::uint32_t destinationAddress() const;
        // TBD: contract

    std::uint8_t optionsLength() const;
        // TBD: contract

    const std::uint8_t *optionsData() const;
        // TBD: contract

    const std::uint8_t *rest() const;
        // TBD: contract
};

}  // close namespace 'hauberk'

#endif
