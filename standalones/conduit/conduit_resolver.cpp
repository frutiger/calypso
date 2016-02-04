// conduit_resolver.cpp

#include <conduit_resolver.h>

#include <hauberk_bufferutil.h>
#include <hauberk_dnsutil.h>
#include <hauberk_internetutil.h>
#include <hauberk_udputil.h>
#include <trammel_link.h>

#include <iostream>

namespace conduit {

                               // --------------
                               // class Resolver
                               // --------------

// CLASS METHODS
bool Resolver::isNameResolution(trammel::Link::PacketType  type,
                                const std::uint8_t        *data)
{
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;

    if (type != trammel::Link::PacketType::Internet) {
        return false;
    }

    if (IU::Protocol(IU::protocol(data)) != IU::Protocol::UDP) {
        return false;
    }
    const std::uint8_t *udp = IU::payload(data);

    // TBD: distinguish between input and output?
    return UU::Port(UU::destinationPort(udp)) == UU::Port::DNS ||
           UU::Port(UU::sourcePort(udp))      == UU::Port::DNS;
}

// CREATORS
Resolver::Resolver(const RouteAdvisory& adviseRoute)
: d_adviseRoute(adviseRoute)
, d_transactionIdGenerator((std::mt19937(std::random_device()())))
, d_queries()
{
}

// MANIPULATORS
int Resolver::processInputPacket(const Gateways::iterator&  gatewayBegin,
                                 Gateways::size_type        numGateways,
                                 const std::uint8_t        *data,
                                 std::size_t                length)
{
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;
    typedef hauberk::DnsUtil      DU;

    const std::uint8_t *udp = IU::payload(data);
    const std::uint8_t *dns = UU::payload(udp);
    std::uint16_t id        = DU::transactionId(dns);

    auto query = d_queries.insert({ 0, numGateways });
    for (GatewayIndex gatewayIndex = 0;
                      gatewayIndex < numGateways;
                    ++gatewayIndex) {
        std::uint16_t newId;
        do {
            newId = d_transactionIdGenerator();
        } while (d_subQueries.find(newId) != d_subQueries.end());
        d_subQueries[newId] = { query, id, gatewayIndex };

        std::vector<std::uint8_t> newPacket(data, data + length);
        std::uint8_t *udp = IU::payload(newPacket.data());
        UU::updateChecksum(udp);
        std::uint8_t *dns = UU::payload(udp);
        DU::setTransactionId(dns, newId);

        auto gateway = gatewayBegin + gatewayIndex;
        if (gateway->d_link.send(trammel::Link::PacketType::Internet,
                                 newPacket.data(),
                                 newPacket.size())) {
            return -1;
        }
    }

    return 0;
}

void Resolver::processOutputPacket(std::vector<std::uint8_t> *newPacket,
                                   const std::uint8_t        *data,
                                   std::size_t                length)
{
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;
    typedef hauberk::DnsUtil      DU;

    const std::uint8_t *udp = IU::payload(data);
    const std::uint8_t *dns = UU::payload(udp);
    auto subQuery = d_subQueries.find(DU::transactionId(dns));
    if (subQuery == d_subQueries.end()) {
        // TBD: warn on unexpected DNS response?
        return;
    }

    for (auto responseIndex = 0;
              responseIndex < DU::numResponses(dns);
            ++responseIndex) {
        auto                recordIndex = DU::numQueries(dns) + responseIndex;
        const std::uint8_t *record      = DU::findRecord(dns, recordIndex);
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

        Queries::iterator query = subQuery->second.d_query;
        auto preferredGateway   = std::min(query->d_preferredGatewayIndex,
                                           subQuery->second.d_gatewayIndex);
        query->d_preferredGatewayIndex = preferredGateway;
        d_subQueries.erase(subQuery);

        --query->d_gatewaysRemaining;
        if (query->d_gatewaysRemaining != 0) {
            continue;
        }

        d_adviseRoute(address, query->d_preferredGatewayIndex);

        std::string addrString, gwyString;
        hauberk::InternetAddressUtil::toDisplay(&addrString, address);
        std::cout << "Name " << name << " "
                     "address " << addrString << " "
                     "gateway " << query->d_preferredGatewayIndex << '\n';
        d_queries.erase(query);
    }

    newPacket->assign(data, data + length);
    // TBD: do we need this?
    // IU::setSourceAddress(newPacket.data(), ...);
    std::uint8_t *newUdp = IU::payload(newPacket.data());
    std::uint8_t *newDns = UU::payload(newUdp);
    DU::setTransactionId(newUdp, query->second.d_originalTransactionId);
    UU::updateChecksum(newUdp);
}

}  // close namespace 'conduit'

