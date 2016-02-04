// conduit_listener.cpp

#include <conduit_listener.h>

#include <maxwell_queue.h>
#include <hauberk_bufferutil.h>
#include <hauberk_dnsutil.h>
#include <hauberk_internetutil.h>
#include <hauberk_internetaddressutil.h>
#include <hauberk_udputil.h>

#include <array>
#include <cassert>
#include <ostream>

#include <sys/socket.h>

#include <iostream>

namespace conduit {

                               // --------------
                               // class Listener
                               // --------------

// MODIFIERS
int Listener::processInputPacket(trammel::Link::PacketType  type,
                                 const std::uint8_t        *data,
                                 std::size_t                length)
{
    typedef hauberk::InternetUtil IU;

    if (Resolver::isNameResolution(type, data)) {
        return d_resolver.processInputPacket(d_gateways.begin(),
                                             d_gateways.size(),
                                             data,
                                             length);
    }

    std::uint32_t destination = IU::destinationAddress(data);
    Routes::const_iterator route = d_routes.find(destination);
    if (route == d_routes.end()) {
        std::cout << "No route for " << destination << '\n';
        return 0;
    }
    auto *gateway = &d_gateways[route->second];

    std::vector<std::uint8_t> packet(data, data + length);
    IU::setDestinationAddress(packet.data(), gateway->d_address);
    IU::updateChecksum(packet.data());
    return gateway->d_link.send(trammel::Link::PacketType::Internet,
                               packet.data(),
                               packet.size());
}

int Listener::processOutputPacket(trammel::Link::PacketType  type,
                                  const std::uint8_t        *data,
                                  std::size_t                length)
{
    if (Resolver::isNameResolution(type, data)) {
        std::vector<std::uint8_t> newPacket;
        d_resolver.processOutputPacket(&newPacket, data, length);
    }

    return 0;
}

// CREATORS
Listener::Listener()
: d_input()
, d_gateways()
, d_resolver(std::bind(&Listener::routeAdvised,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2))
, d_routes()
{
}

// MANIPULATORS
int Listener::open(std::ostream&                          errorStream,
                   const maxwell::Queue&                  queue,
                   const ArgumentUtil::Simplex&           input,
                   ArgumentUtil::Duplexes::const_iterator output,
                   ArgumentUtil::Duplexes::const_iterator outputEnd)
{
    if (d_input.open(errorStream,
                     queue,
                     input.d_interfaceName,
                     std::bind(&Listener::processInputPacket,
                               this,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::placeholders::_3),
                     10,
                     65536,
                     true)) {
        return -1;
    }

    for (auto gateway = output; gateway != outputEnd; ++gateway) {
        d_gateways.resize(d_gateways.size() + 1);
        if (d_gateways.back().d_link.open(
                                      errorStream,
                                      queue,
                                      gateway->d_interfaceName,
                                      std::bind(&Listener::processOutputPacket,
                                                this,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                std::placeholders::_3),
                                      10,
                                      65536,
                                      true)) {
            return -1;
        }
    }

    return 0;
}

}  // close namespace 'conduit'

