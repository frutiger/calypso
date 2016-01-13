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
    explicit Dns(const std::uint8_t *buffer);

}  // close namespace 'conduit'

#endif
