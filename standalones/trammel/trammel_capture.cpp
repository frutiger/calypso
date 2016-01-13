// trammel_capture.cpp

#include <trammel_capture.h>

#include <hauberk_ethernet.h>
#include <hauberk_internet.h>
#include <hauberk_loopback.h>
#include <maxwell_queue.h>

#include <cassert>

#include <pcap/pcap.h>

namespace trammel {

namespace {
static const std::uint32_t PROTOCOL_FAMILY_INTERNET = 2;
}  // close unnamed namespace

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
int Capture::read(hauberk::Internet *internet)
{
    struct pcap_pkthdr *header;
    const std::uint8_t *data;

    while (true) {
        if (pcap_next_ex(d_handle.get(), &header, &data) != 1) {
            return -1;
        }

        Metadata metadata(header);

        // TBD: better error handling
        assert(metadata.capturedDataLength() == metadata.dataLength());

        // TBD: install BPF filter programs instead of checking the packet in
        // user space
        switch (d_datalinkType) {
          case DLT_NULL: {
            hauberk::Loopback loopback(data);
            if (PROTOCOL_FAMILY_INTERNET != loopback.protocolFamily()) {
                continue;
            }
            *internet = hauberk::Internet(loopback.payload());
          } break;

          case DLT_EN10MB: {
            hauberk::Ethernet ethernet(data);
            if (d_hardwareAddress != ethernet.destinationAddress()) {
                continue;
            }
            *internet = hauberk::Internet(ethernet.payload());
          } break;
        }

        break;
    }

    return 0;
}

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
    char         createErrorBuffer[PCAP_ERRBUF_SIZE];
    struct pcap *capture = pcap_create(d_interface.c_str(), createErrorBuffer);
    if (!capture) {
        errorStream << "Failed to create pcap: " << createErrorBuffer;
        return -1;
    }
    else {
        d_handle.reset(capture);
    }

    int timeoutRc = pcap_set_timeout(d_handle.get(), timeoutMilliseconds);
    assert(!timeoutRc);

    int snapshotRc = pcap_set_snaplen(d_handle.get(), snapshotLength);
    assert(!snapshotRc);

    char nonblockErrorBuffer[PCAP_ERRBUF_SIZE];
    if (pcap_setnonblock(d_handle.get(),
                         nonblock,
                         nonblockErrorBuffer) == -1) {
        errorStream << "Failed to set nonblock: " << nonblockErrorBuffer;
        return -1;
    }

    int activationRc = pcap_activate(d_handle.get());
    if (activationRc) {
        switch (activationRc) {
            case PCAP_ERROR:
                errorStream << "unknown activation error";
            case PCAP_ERROR_ACTIVATED:
                errorStream << "already activated";
            case PCAP_ERROR_IFACE_NOT_UP:
                errorStream << "interface not up";
            case PCAP_ERROR_NO_SUCH_DEVICE:
                errorStream << "no such device";
            case PCAP_ERROR_PERM_DENIED:
                errorStream << "permission denied";
            case PCAP_ERROR_PROMISC_PERM_DENIED:
                errorStream << "promiscuous mode permission denied";
            case PCAP_ERROR_RFMON_NOTSUP:
                errorStream << "monitor mode not supported";
            case PCAP_WARNING:
                errorStream << "unknown activation warning";
            case PCAP_WARNING_PROMISC_NOTSUP:
                errorStream << "promiscuous mode not supported";
            case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
                errorStream << "timestamp format not supported";
            default:
                errorStream << "unknown activation error";
        }
        return -1;
    }

    d_datalinkType = pcap_datalink(d_handle.get());

    int descriptor = pcap_get_selectable_fd(d_handle.get());
    if (descriptor == -1) {
        errorStream << "Failed to get selectable file descriptor";
        return -1;
    }

    if (queue.setReadHandler(errorStream,
                             &d_readHandle,
                             static_cast<std::uintptr_t>(descriptor),
                             &d_eventHandler)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'trammel'

