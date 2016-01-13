// trammel_duplex.cpp

#include <trammel_duplex.h>

#include <hauberk_bufferutil.h>
#include <hauberk_internet.h>
#include <hauberk_loopback.h>
#include <maxwell_queue.h>

#include <cassert>
#include <random>

#include <pcap/pcap.h>

namespace trammel {

namespace {
static const std::uint32_t PROTOCOL_FAMILY_INTERNET = 2;
}  // close unnamed namespace

                                // ------------
                                // class Duplex
                                // ------------

// PRIVATE CLASS METHODS
int Duplex::dispatchEvent(std::uintptr_t, void *userData)
{
    return static_cast<Duplex *>(userData)->packetsReady();
}

// MODIFIERS
int Duplex::read(hauberk::Internet *internet)
{
    struct pcap_pkthdr *header;
    const std::uint8_t *data;

    while (true) {
        if (pcap_next_ex(d_handle.get(), &header, &data) != 1) {
            return -1;
        }

        // TBD: better error handling
        assert(header->caplen == header->len);

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

int Duplex::packetsReady()
{
    hauberk::Internet internet(0);
    while (!read(&internet)) {
        if (d_packetHandler(internet, d_handlerUserData)) {
            return -1;
        }
    }
    return 0;
}

// CLASS METHODS
Duplex Duplex::create(const std::string&  interface,
                      PacketHandler       packetHandler,
                      void               *handlerUserData)
{
    typedef std::independent_bits_engine<std::mt19937, 8, std::uint8_t> Engine;

    Engine                     engine((std::mt19937(std::random_device()())));
    hauberk::Ethernet::Address hardwareAddress;
    std::generate(hardwareAddress.begin(), hardwareAddress.end(), engine);
    return Duplex(interface, hardwareAddress, packetHandler, handlerUserData);
}

// CREATORS
Duplex::Duplex(const std::string&                 interface,
               const hauberk::Ethernet::Address&  hardwareAddress,
               PacketHandler                      packetHandler,
               void                              *handlerUserData)
: d_interface(interface)
, d_hardwareAddress(hardwareAddress)
, d_packetHandler(packetHandler)
, d_handlerUserData(handlerUserData)
, d_handle(0, &pcap_close)
, d_datalinkType()
, d_eventHandler(&Duplex::dispatchEvent, this)
, d_readHandle()
{
}

// MANIPULATORS
int Duplex::open(std::ostream&         errorStream,
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

int Duplex::send(const hauberk::Internet& internet)
{
    typedef std::vector<std::uint8_t> Data;

    Data               packet;
    Data::iterator p = packet.begin();
    if (!d_hardwareAddress.empty()) {
        packet.reserve(d_hardwareAddress.size() + internet.length());
        p = std::copy(d_hardwareAddress.begin(), d_hardwareAddress.end(), p);
    }
    else {
        packet.reserve(sizeof(PROTOCOL_FAMILY_INTERNET) + internet.length());
        hauberk::BufferUtil::copy(packet.data(),
                                  PROTOCOL_FAMILY_INTERNET);
        p = p + sizeof(PROTOCOL_FAMILY_INTERNET);
    }

    p = std::copy(internet.buffer(), internet.buffer() + internet.length(), p);
    if (pcap_sendpacket(d_handle.get(), packet.data(), packet.size())) {
        return -1;
    }

    return 0;
}

}  // close namespace 'trammel'

