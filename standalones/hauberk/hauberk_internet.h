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
    // CREATORS
    explicit Internet(const uint8_t *buffer);
        // TBD: contract

    // ACCESSORS
    uint8_t version() const;
        // TBD: contract

    uint16_t headerLength() const;
        // TBD: contract

    uint8_t differentiatedSercicesCodePoint() const;
        // TBD: contract

    uint8_t explicitCongestioNotification() const;
        // TBD: contract

    uint16_t packetLength() const;
        // TBD: contract

    uint16_t identification() const;
        // TBD: contract

    uint8_t flags() const;
        // TBD: contract

    uint16_t fragmentOffset() const;
        // TBD: contract

    uint8_t timeToLive() const;
        // TBD: contract

    uint8_t protocol() const;
        // TBD: contract

    uint16_t headerChecksum() const;
        // TBD: contract

    uint32_t sourceAddress() const;
        // TBD: contract

    uint32_t destinationAddress() const;
        // TBD: contract

    uint32_t optionsLength() const;
        // TBD: contract

    const uint8_t *optionsData() const;
        // TBD: contract

    const uint8_t *rest() const;
        // TBD: contract
};

}  // close namespace 'hauberk'

#endif
