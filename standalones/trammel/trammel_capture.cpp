// trammel_capture.cpp

#include <trammel_capture.h>

#include <hauberk_ethernet.h>
#include <hauberk_internet.h>
#include <hauberk_loopback.h>
#include <maxwell_queue.h>

#include <cassert>

#include <pcap/pcap.h>

namespace trammel {

                          // -----------------------
                          // class Capture::Metadata
                          // -----------------------

// CREATORS
Capture::Metadata::Metadata()
: d_header()
{
}

Capture::Metadata::Metadata(const struct pcap_pkthdr *header)
: d_header(header)
{
}

// ACCESSORS
std::chrono::time_point<std::chrono::system_clock>
Capture::Metadata::time() const
{
    return std::chrono::time_point<std::chrono::system_clock>(
                              std::chrono::seconds(d_header->ts.tv_sec) +
                              std::chrono::microseconds(d_header->ts.tv_usec));
}

std::uint32_t Capture::Metadata::capturedDataLength() const
{
    return d_header->caplen;
}

std::uint32_t Capture::Metadata::dataLength() const
{
    return d_header->len;
}

                               // -------------
                               // class Capture
                               // -------------

// PRIVATE CLASS METHODS
int Capture::dispatchEvent(std::uintptr_t, void *userData)
{
    return static_cast<Capture *>(userData)->packetsReady();
}

// MODIFIERS

int Capture::packetsReady()
{
    hauberk::Internet internet(0);
    while (!read(&internet)) {
        if (d_packetHandler(internet, d_handlerUserData)) {
            return -1;
        }
    }
    return 0;
}

// CREATORS
Capture::Capture(const std::string&                 interface,
                 const hauberk::Ethernet::Address&  hardwareAddress,
                 PacketHandler                      packetHandler,
                 void                              *handlerUserData)
: d_interface(interface)
, d_hardwareAddress(hardwareAddress)
, d_packetHandler(packetHandler)
, d_handlerUserData(handlerUserData)
, d_handle(0, &pcap_close)
, d_datalinkType()
, d_eventHandler(&Capture::dispatchEvent, this)
, d_readHandle()
{
}

// MANIPULATORS
int Capture::open(std::ostream&         errorStream,
                  int                   timeoutMilliseconds,
                  int                   snapshotLength,
                  int                   nonblock,
                  const maxwell::Queue& queue)
{
}

}  // close namespace 'trammel'

