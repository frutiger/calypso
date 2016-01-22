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
                                 std::uint32_t       gateway)
{
    std::cout << "got response from " << gateway << ": ";
    std::for_each(packetData,
                  packetData + packetLength,
                  [](std::uint8_t octet) { std::cout << (int)octet << ' '; });
    std::cout << '\n';
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
Listener::Listener(const ArgumentUtil::Simplex&           simplex,
                   ArgumentUtil::Duplexes::const_iterator duplex,
                   ArgumentUtil::Duplexes::const_iterator duplexEnd)
: d_duplex(simplex.d_interfaceName,
           simplex.d_address,
           0,
           std::bind(&Listener::processPacket,
                     this,
                     std::placeholders::_1,
                     std::placeholders::_2,
                     std::placeholders::_3))
, d_resolver(duplex, duplexEnd)
{
}

// MANIPULATORS
int Listener::open(std::ostream& errorStream, const maxwell::Queue& queue)
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

