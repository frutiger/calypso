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
int Listener::processDnsResponse(const std::uint8_t *packetData,
                                 std::size_t         packetLength,
                                 std::size_t         gatewayIndex)
{
    typedef hauberk::EthernetUtil EU;
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;
    typedef hauberk::DnsUtil      DU;

    const std::uint8_t *udp = IU::payload(packetData);
    const std::uint8_t *dns = UU::payload(udp);

    std::uint16_t numQueries = DU::numQueries(dns);
    for (auto i = 0; i < DU::numResponses(dns); ++i) {
        const std::uint8_t *record = DU::findRecord(dns, numQueries + i);
        if (DU::Type(DU::recordType(record)) != DU::Type::A) {
            continue;
        }

        if (DU::Class(DU::recordClass(record)) != DU::Class::INTERNET) {
            continue;
        }

        if (DU::recordDataLength(record) != 4) {
            continue;
        }

        std::uint32_t address;
        hauberk::BufferUtil::copy(&address, DU::recordData(record));

        std::string name;
        DU::walkLabels(dns, record, [&name](auto label, auto length) {
            std::copy(label, label + length, std::back_inserter(name));
            name.push_back('.');
        });
        Queries::iterator query = d_queries.find(name);
        if (query == d_queries.end()) {
            continue;
        }

        if (query->second.d_gatewaysRemaining == d_gateways.size()) {
            query->second.d_preferredGatewayIndex = gatewayIndex;
        }
        else {
            query->second.d_preferredGatewayIndex =
                                std::min(query->second.d_preferredGatewayIndex,
                                         gatewayIndex);
        }

        --query->second.d_gatewaysRemaining;
        if (query->second.d_gatewaysRemaining != 0) {
            continue;
        }

        d_routes[address] = &d_gateways[query->second.d_preferredGatewayIndex];

        std::string addrString, gwyString;
        hauberk::InternetAddressUtil::toDisplay(&addrString, address);
        hauberk::InternetAddressUtil::toDisplay(&gwyString,
                                                d_routes[address]->d_address);
        std::cout << "Name " << name << " "
                     "address " << addrString << " "
                     "gateway " << gwyString << '\n';
        d_queries.erase(query);
    }

    d_input.send(EU::Type::INTERNET,
                 packetData,
                 packetLength);
    return 0;
}

int Listener::processDnsRequest(const std::uint8_t *request,
                                std::size_t         requestLength)
{
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;
    typedef hauberk::DnsUtil      DU;

    const std::uint8_t *udp = IU::payload(request);
    const std::uint8_t *dns = UU::payload(udp);

    for (auto i = 0; i < DU::numQueries(dns); ++i) {
        const std::uint8_t *record = DU::findRecord(dns, i);

        if (DU::Type(DU::recordType(record)) != DU::Type::A) {
            continue;
        }

        if (DU::Class(DU::recordClass(record)) != DU::Class::INTERNET) {
            continue;
        }

        std::string name;
        DU::walkLabels(dns, record, [&name](auto label, auto length) {
            std::copy(label, label + length, std::back_inserter(name));
            name.push_back('.');
        });
        d_queries[name] = { 0, d_gateways.size() };
    }

    return d_resolver.resolve(request,
                              requestLength,
                              std::bind(&Listener::processDnsResponse,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3));
}

int Listener::processPacket(hauberk::EthernetUtil::Type  type,
                            const std::uint8_t          *packetData,
                            std::size_t                  packetLength)
{
    typedef hauberk::EthernetUtil EU;
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;

    if (type != hauberk::EthernetUtil::Type::INTERNET) {
        return 0;
    }

    if (IU::Protocol(IU::protocol(packetData)) == IU::Protocol::UDP) {
        const std::uint8_t *udp = IU::payload(packetData);
        if (UU::Port(UU::destinationPort(udp)) == UU::Port::DNS) {
            return processDnsRequest(packetData, packetLength);
        }
    }
    else {
        std::uint32_t destination = IU::destinationAddress(packetData);
        Routes::const_iterator route = d_routes.find(destination);
        if (route == d_routes.end()) {
            std::cout << "No route for " << destination << '\n';
            return 0;
        }

        std::vector<std::uint8_t> newPacket(packetData,
                                            packetData + packetLength);
        IU::setDestinationAddress(newPacket.data(), route->second->d_address);
        IU::updateChecksum(newPacket.data());
        return route->second->d_duplex.send(EU::Type::INTERNET,
                                            newPacket.data(),
                                            newPacket.size());
    }
    return 0;
}

// CREATORS
Listener::Listener(const ArgumentUtil::Simplex&           input,
                   ArgumentUtil::Duplexes::const_iterator output,
                   ArgumentUtil::Duplexes::const_iterator outputEnd)
: d_input(input.d_interfaceName,
          input.d_source,
          input.d_source,
          std::bind(&Listener::processPacket,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3))
, d_gateways()
, d_resolver(output, outputEnd)
, d_queries()
, d_routes()
{
    std::transform(output,
                   outputEnd,
                   std::back_inserter(d_gateways),
                   [](auto& duplex) -> Gateway {
                       return {
                           duplex.d_tunnel.d_destination, {
                               duplex.d_interfaceName,
                               duplex.d_tunnel.d_source,
                               duplex.d_tunnel.d_destination,
                               [](auto&&, auto&&, auto&&) -> int { return 0; },
                           },
                       };
                   });
}

// MANIPULATORS
int Listener::open(std::ostream& errorStream, const maxwell::Queue& queue)
{
    if (d_input.open(errorStream, 10, 65536, true, queue)) {
        return -1;
    }

    for (auto&& gateway: d_gateways) {
        if (gateway.d_duplex.open(errorStream, 10, 65536, true, queue)) {
            return -1;
        }
    }

    if (d_resolver.open(errorStream, 10, 65536, true, queue)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'conduit'

