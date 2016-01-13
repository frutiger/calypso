// hauberk_socketaddress.h
#ifndef HAUBERK_SOCKETADDRESS
#define HAUBERK_SOCKETADDRESS

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif

struct sockaddr;

namespace hauberk {

                            // ===================
                            // class SocketAddress
                            // ===================

class SocketAddress {
    // DATA
    const std::shared_ptr<const struct sockaddr> d_address;

  public:
    // CREATORS
    explicit SocketAddress(
                        const std::shared_ptr<const struct sockaddr>& address);
        // TBD: contract

    // ACCESSORS
    bool isInternet() const;
        // TBD: contract

    std::uint32_t theInternet() const;
        // TBD: contract

    // TBD: extend for other socket address types
};

}  // close namespace 'hauberk'

#endif
