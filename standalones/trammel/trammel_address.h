// trammel_address.h
#ifndef TRAMMEL_ADDRESS
#define TRAMMEL_ADDRESS

#ifndef INCLUDED_MEMORY
#include <memory>
#endif

struct pcap_addr;

namespace hauberk { class SocketAddress; }
namespace trammel {

                               // =============
                               // class Address
                               // =============

class Address {
    // DATA
    std::shared_ptr<const struct pcap_addr> d_address;

  public:
    // CREATORS
    explicit Address(const std::shared_ptr<const struct pcap_addr>& address);
        // TBD: contract

    // ACCESSORS
    hauberk::SocketAddress broadcastAddress() const;
        // TBD: contract

    hauberk::SocketAddress destinationAddress() const;
        // TBD: contract

    bool hasBroadcastAddress() const;
        // TBD: contract

    bool hasDestinationAddress() const;
        // TBD: contract

    bool hasSubnetworkMask() const;
        // TBD: contract

    hauberk::SocketAddress sendAddress() const;
        // TBD: contract

    hauberk::SocketAddress subnetworkMask() const;
        // TBD: contract
};

}  // close namespace 'trammel'

#endif

