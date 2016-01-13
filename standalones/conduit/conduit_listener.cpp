// conduit_listener.cpp

#include <conduit_listener.h>

#include <maxwell_queue.h>
#include <hauberk_internet.h>
#include <hauberk_internetaddressutil.h>
#include <hauberk_loopback.h>
#include <hauberk_udp.h>

#include <array>
#include <cassert>
#include <ostream>

#include <sys/socket.h>

#include <iostream>

namespace conduit {

                               // --------------
                               // class Listener
                               // --------------

// PRIVATE CLASS METHODS
int Listener::dispatchPacket(const hauberk::Internet&  internet,
                             void                     *userData)
{
    return static_cast<Listener *>(userData)->processPacket(internet);
}

// MODIFIERS
int Listener::processDnsRequest(const hauberk::Internet& internet)
{
    //d_resolver.resolve
    std::cout << "dns request from " << internet.sourceAddress() << '\n';
    return 0;
}

int Listener::processPacket(const hauberk::Internet& internet)
{
    return 0;
}

// CREATORS
Listener::Listener(
              const std::string&                                 interface,
              std::uint32_t                                      address,
              ArgumentParser::InterfaceAddresses::const_iterator endpoint,
              ArgumentParser::InterfaceAddresses::const_iterator endpointEnd)
: d_address(address)
, d_duplex(trammel::Duplex::create(interface,
                                   &Listener::dispatchPacket,
                                   this))
, d_resolver(endpoint, endpointEnd)
{
    // TBD: silence warning
    ++d_address;
}

// MANIPULATORS
int Listener::activate(std::ostream& errorStream, const maxwell::Queue& queue)
{
    if (d_duplex.open(errorStream, 10, 65536, true, queue)) {
        return -1;
    }

    if (d_resolver.open(errorStream, 10, 65536, true, queue)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'conduit'

