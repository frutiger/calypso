// conduit_resolver.cpp

#include <conduit_resolver.h>

#include <hauberk_dnsutil.h>
#include <hauberk_internetutil.h>
#include <hauberk_udputil.h>
#include <trammel_duplex.h>

#include <iostream>

namespace conduit {

                               // --------------
                               // class Resolver
                               // --------------

// MODIFIERS
int Resolver::processPacket(hauberk::EthernetUtil::Type  type,
                            const std::uint8_t          *packetData,
                            std::size_t                  packetLength)
{
    typedef hauberk::EthernetUtil EU;
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;
    typedef hauberk::DnsUtil      DU;

    switch (type) {
      case EU::Type::ARP: {
      } return 0;

      case EU::Type::INTERNET: {
        if (IU::Protocol(IU::protocol(packetData)) != IU::Protocol::UDP) {
            return 0;
        }

        const std::uint8_t *udp = IU::payload(packetData);
        const std::uint8_t *dns = UU::payload(udp);
        Transactions::const_iterator transaction =
                                   d_transactions.find(DU::transactionId(dns));
        if (transaction == d_transactions.end()) {
            return 0;
        }

        std::vector<std::uint8_t> newResponse(packetData,
                                              packetData + packetLength);
        IU::setSourceAddress(newResponse.data(),
                             std::get<1>(transaction->second));
        std::uint8_t *newUdp = IU::payload(newResponse.data());
        std::uint8_t *newDns = UU::payload(newUdp);
        std::cout << "mapping from " << (int)transaction->first << " to "
                  << (int)std::get<0>(transaction->second) << '\n';
        DU::setTransactionId(newDns, std::get<0>(transaction->second));
        std::get<2>(transaction->second)(newResponse.data(),
                                         newResponse.size(),
                                         std::get<1>(transaction->second));
        d_transactions.erase(transaction);
      } return 0;

      default:
        return 0;
    }
}

// CREATORS
Resolver::Resolver(ArgumentUtil::Duplexes::const_iterator output,
                   ArgumentUtil::Duplexes::const_iterator outputEnd)
: d_duplexes()
, d_engine((std::mt19937(std::random_device()())))
, d_transactions()
{
    for (; output != outputEnd; ++output) {
        d_duplexes.emplace_back(output->d_interfaceName,
                                output->d_tunnel.d_source,
                                output->d_tunnel.d_destination,
                                std::bind(&Resolver::processPacket,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3));
    }
}

// MANIPULATORS
int Resolver::open(std::ostream&         errorStream,
                   int                   timeoutMilliseconds,
                   int                   snapshotLength,
                   int                   nonblock,
                   const maxwell::Queue& queue)
{
    for (auto& duplex: d_duplexes) {
        if (duplex.open(errorStream,
                        timeoutMilliseconds,
                        snapshotLength,
                        nonblock,
                        queue)) {
            return -1;
        }
    }
    return 0;
}

int Resolver::resolve(const std::uint8_t *request,
                      std::size_t         requestLength,
                      const Handler&      handler)
{
    typedef hauberk::EthernetUtil EU;
    typedef hauberk::InternetUtil IU;
    typedef hauberk::UdpUtil      UU;
    typedef hauberk::DnsUtil      DU;

    const std::uint8_t *udp     = IU::payload(request);
    const std::uint8_t *dns     = UU::payload(udp);
    std::uint16_t transactionId = DU::transactionId(dns);
    for (decltype(d_duplexes.size()) i = 0; i != d_duplexes.size(); ++i) {
        std::uint16_t newTransactionId = d_engine();
        while (true) {
            if (d_transactions.insert({ newTransactionId,
                                        { transactionId,
                                          i,
                                          handler }}).second) {
                break;
            }
        }

        std::vector<std::uint8_t> newRequest(request, request + requestLength);
        std::uint8_t *udp = IU::payload(newRequest.data());
        UU::updateChecksum(udp);
        std::uint8_t *dns = UU::payload(udp);
        DU::setTransactionId(dns, newTransactionId);

        if (d_duplexes[i].send(EU::Type::INTERNET,
                               newRequest.data(),
                               newRequest.size())) {
            return -1;
        }
    }
    return 0;
}

}  // close namespace 'conduit'

