// hauberk_socketaddress.cpp

#include <hauberk_socketaddress.h>

#include <algorithm>

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
    struct sockaddr_in internet;
    std::copy_n(static_cast<const char *>(static_cast<const void *>(
                                                             d_address.get())),
                sizeof(sockaddr_in),
                static_cast<char *>(static_cast<void *>(&internet)));
    return internet.sin_addr.s_addr;
}

}  // close namespace 'hauberk'

