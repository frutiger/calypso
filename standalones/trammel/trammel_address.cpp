// trammel_address.cpp

#include <trammel_address.h>

#include <hauberk_socketaddress.h>

#include <pcap/pcap.h>

namespace trammel {

                               // -------------
                               // class Address
                               // -------------

// CREATORS
Address::Address(const std::shared_ptr<const struct pcap_addr>& address)
: d_address(address)
{
}

// ACCESSORS
hauberk::SocketAddress Address::broadcastAddress() const
{
    std::shared_ptr<const struct sockaddr> address(d_address,
                                                   d_address->broadaddr);
    return hauberk::SocketAddress(address);
}

hauberk::SocketAddress Address::destinationAddress() const
{
    std::shared_ptr<const struct sockaddr> address(d_address,
                                                   d_address->dstaddr);
    return hauberk::SocketAddress(address);
}

bool Address::hasBroadcastAddress() const
{
    return d_address->broadaddr;
}

bool Address::hasDestinationAddress() const
{
    return d_address->dstaddr;
}

bool Address::hasSubnetworkMask() const
{
    return d_address->netmask;
}

hauberk::SocketAddress Address::sendAddress() const
{
    std::shared_ptr<const struct sockaddr> address(d_address, d_address->addr);
    return hauberk::SocketAddress(address);
}

hauberk::SocketAddress Address::subnetworkMask() const
{
    std::shared_ptr<const struct sockaddr> address(d_address,
                                                   d_address->netmask);
    return hauberk::SocketAddress(address);
}

}  // close namespace 'trammel'

