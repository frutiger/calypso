// hauberk_dns.h
#ifndef HAUBERK_DNS
#define HAUBERK_DNS

namespace hauberk {

                                 // =========
                                 // class Dns
                                 // =========

class Dns {
    // DATA
    const std::uint8_t *d_buffer;

  public:
    // CREATORS
    explicit Dns(const std::uint8_t *buffer);
        // TBD: contract

    // ACCESSORS
    std::uint16_t transactionId() const;
        // TBD: contract

    // TBD: add remaining accessors

}  // close namespace 'hauberk'

#endif
