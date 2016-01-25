// conduit_listener.cpp

#include <conduit_listener.h>

#include <maxwell_queue.h>
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
                                 std::size_t         duplexIndex)
{
    typedef hauberk::EthernetUtil EU;

    std::cout << "got response from " << duplexIndex << '\n';
    d_input.send(EU::Type::INTERNET,
                 packetData,
                 packetLength);
    return 0;
}

int Listener::processDnsRequest(const std::uint8_t *request,
                                std::size_t         requestLength)
{
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
, d_resolver(output, outputEnd)
{
}

// MANIPULATORS
int Listener::open(std::ostream& errorStream, const maxwell::Queue& queue)
{
    if (d_input.open(errorStream, 10, 65536, true, queue)) {
        return -1;
    }

    if (d_resolver.open(errorStream, 10, 65536, true, queue)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'conduit'

