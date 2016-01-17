// hauberk_socketaddress.cpp

#include <hauberk_socketaddress.h>

#include <sys/types.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace hauberk {

                            // -------------------
                            // class SocketAddress
                            // -------------------

// CREATORS
SocketAddress::SocketAddress(
                         const std::shared_ptr<const struct sockaddr>& address)
: d_address(address)
{
}

// ACCESSORS
bool SocketAddress::isInternet() const
{
    return d_address->sa_family == AF_INET;
}

std::uint32_t SocketAddress::theInternet() const
{
    const struct sockaddr_in *internet =
                     static_cast<const struct sockaddr_in *>(
                     static_cast<const void               *>(d_address.get()));
    return internet->sin_addr.s_addr;
}

bool SocketAddress::isLink() const
{
    return d_address->sa_family == AF_LINK;
}

EthernetUtil::Address SocketAddress::theLink() const
{
    EthernetUtil::Address address;
    const struct sockaddr_dl *datalink =
                     static_cast<const struct sockaddr_dl *>(
                     static_cast<const void               *>(d_address.get()));
    std::copy(LLADDR(datalink),
              LLADDR(datalink) + datalink->sdl_alen,
              address.begin());
    return address;
}

}  // close namespace 'hauberk'

