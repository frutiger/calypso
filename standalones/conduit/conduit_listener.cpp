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
int Listener::dispatchEvent(std::uintptr_t, void *userData)
{
    return static_cast<Listener *>(userData)->packetsReady();
}

// MODIFIERS
int Listener::processDnsRequest(const hauberk::Internet& internet)
{
    std::cout << "dns request from " << internet.sourceAddress() << '\n';
    return 0;
}

int Listener::processLoopbackPacket(const std::uint8_t *data)
{
    hauberk::Loopback loopback(data);
    if (loopback.protocolFamily() != PF_INET) {
        return 0;
    }

    hauberk::Internet internet(loopback.rest());
    if (internet.version() != 4) {
        return 0;
    }

    if (hauberk::Internet::Protocol(internet.protocol()) ==
                                            hauberk::Internet::Protocol::UDP) {
        hauberk::Udp udp(internet.rest());
        if (hauberk::Udp::Port(udp.destinationPort()) ==
                                                     hauberk::Udp::Port::DNS) {
            return processDnsRequest(internet);
        }
    }

    return 0;
}

int Listener::processEthernetPacket(const std::uint8_t *data)
{
    // TBD: implement
    std::uint8_t c = data[0] + 1;
    c++;
    return 0;
}

int Listener::processPacket(const trammel::CaptureMetadata *metadata,
                            const std::uint8_t             *data)
{
    if (metadata->capturedDataLength() != metadata->dataLength()) {
        return -1;
    }

    switch (d_linkType) {
      case trammel::Capture::LinkType::Loopback:
        return processLoopbackPacket(data);

      case trammel::Capture::LinkType::Ethernet:
        return processEthernetPacket(data);

      case trammel::Capture::LinkType::Unknown:
        return -1;
    }
}

int Listener::packetsReady()
{
    assert(d_activated);

    const trammel::CaptureMetadata *metadata;
    const std::uint8_t             *data;
    while (!d_capture.read(&metadata, &data)) {
        if (processPacket(metadata, data)) {
            return -1;
        }
    }
    return 0;
}

// CREATORS
Listener::Listener(
              const std::string&                                 interface,
              std::uint32_t                                      address,
              ArgumentParser::InterfaceAddresses::const_iterator endpoint,
              ArgumentParser::InterfaceAddresses::const_iterator endpointEnd)
: d_interface(interface)
, d_address(address)
, d_eventHandler(&Listener::dispatchEvent, this)
, d_readHandle()
, d_capture()
, d_activated()
, d_linkType()
, d_resolver(endpoint, endpointEnd)
{
    // TBD: silence warning
    ++d_address;
}

// MANIPULATORS
int Listener::activate(std::ostream& errorStream, const maxwell::Queue& queue)
{
    assert(!d_activated);

    if (d_capture.open(errorStream, d_interface)) {
        return -1;
    }

    if (d_capture.setNonblock(errorStream, true)) {
        return -1;
    }

    d_capture.setTimeout(10);
    if (d_capture.activate()) {
        errorStream << "Failed to activate listener\n";
        return -1;
    }
    d_activated = true;

    d_linkType = d_capture.linkType();
    std::uintptr_t listenDescriptor;
    if (d_capture.fileDescriptor(&listenDescriptor)) {
        errorStream << "Failed to get listener descriptor\n";
        return -1;
    }

    if (queue.setReadHandler(errorStream,
                             &d_readHandle,
                             listenDescriptor,
                             &d_eventHandler)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'conduit'

