// trammel_duplex.cpp

#include <trammel_duplex.h>

#include <hauberk_arputil.h>
#include <hauberk_ethernetutil.h>
#include <hauberk_loopbackutil.h>

#include <cassert>
#include <random>

#include <pcap/pcap.h>

namespace trammel {

                                // ------------
                                // class Duplex
                                // ------------

// MODIFIERS
int Duplex::read(hauberk::EthernetUtil::Type  *type,
                 const std::uint8_t          **packetData,
                 std::size_t                  *packetLength)
{
    struct pcap_pkthdr *header;
    const std::uint8_t *data;

    while (true) {
        if (pcap_next_ex(d_pcapHandle.get(), &header, &data) != 1) {
            return -1;
        }

        // TBD: better error handling
        assert(header->caplen == header->len);

        typedef hauberk::LoopbackUtil LU;
        typedef hauberk::EthernetUtil EU;

        switch (d_datalinkType) {
          case DLT_NULL: {
            if (LU::Family(LU::protocolFamily(data)) != LU::Family::INTERNET) {
                // TBD: install BPF filter programs instead of checking the
                // packet in user space
                continue;
            }
            *type         = EU::Type::INTERNET;
            *packetData   = LU::payload(data);
            // TBD: something less magic?
            *packetLength = header->caplen - sizeof(LU::Family);
          } break;

          case DLT_EN10MB: {
            EU::Address destinationAddress;
            EU::destinationAddress(&destinationAddress, data);
            if (destinationAddress != d_hardwareAddress) {
                // TBD: install BPF filter programs instead of checking the
                // packet in user space
                continue;
            }
            *type         = EU::type(data);
            *packetData   = EU::payload(data);
            // TBD: something less magic?  Does this include the FCS?
            *packetLength = header->caplen - 14;
          } break;
        }

        break;
    }

    return 0;
}

int Duplex::packetsReady(std::uintptr_t)
{
    hauberk::EthernetUtil::Type  packetType;
    const std::uint8_t          *packetData;
    std::size_t                  packetLength;
    while (!read(&packetType, &packetData, &packetLength)) {
        if (d_packetHandler(packetType, packetData, packetLength)) {
            return -1;
        }
    }
    return 0;
}

// CREATORS
Duplex::Duplex(const std::string&                    interface,
               const hauberk::EthernetUtil::Address& gatewayHardwareAddress,
               std::uint32_t                         address,
               const PacketHandler&                  packetHandler)
: d_interface(interface)
, d_address(address)
, d_hardwareAddress()
, d_packetHandler(packetHandler)
, d_pcapHandle(0, &pcap_close)
, d_datalinkType()
, d_readHandle()
, d_gatewayHardwareAddress(gatewayHardwareAddress)
{
    typedef std::independent_bits_engine<std::mt19937, 8, std::uint8_t> Engine;

    Engine engine((std::mt19937(std::random_device()())));
    std::generate(d_hardwareAddress.begin(), d_hardwareAddress.end(), engine);
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
        d_pcapHandle.reset(capture);
    }

    int timeoutRc = pcap_set_timeout(d_pcapHandle.get(), timeoutMilliseconds);
    assert(!timeoutRc);

    int snapshotRc = pcap_set_snaplen(d_pcapHandle.get(), snapshotLength);
    assert(!snapshotRc);

    char nonblockErrorBuffer[PCAP_ERRBUF_SIZE];
    if (pcap_setnonblock(d_pcapHandle.get(),
                         nonblock,
                         nonblockErrorBuffer) == -1) {
        errorStream << "Failed to set nonblock: " << nonblockErrorBuffer;
        return -1;
    }

    int activationRc = pcap_activate(d_pcapHandle.get());
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

    d_datalinkType = pcap_datalink(d_pcapHandle.get());

    int descriptor = pcap_get_selectable_fd(d_pcapHandle.get());
    if (descriptor == -1) {
        errorStream << "Failed to get selectable file descriptor";
        return -1;
    }

    auto function = std::bind(&Duplex::packetsReady,
                              this,
                              std::placeholders::_1);
    auto handler  = std::make_unique<maxwell::Queue::Handler>(function);
    if (queue.setReadHandler(errorStream,
                             &d_readHandle,
                             &handler,
                             static_cast<std::uintptr_t>(descriptor))) {
        return -1;
    }

    switch (d_datalinkType) {
      case DLT_NULL: {
      } return 0;

      case DLT_EN10MB: {
        /*
        using AU = hauberk::ArpUtil;
        std::vector<std::uint8_t> arpBuffer(28); // TBD: magic constant
        AU::setHardwareType(arpBuffer.data(), AU::HardwareType::ETHERNET);
        AU::setProtocolType(arpBuffer.data(), AU::ProtocolType::INTERNET);
        AU::setHardwareAddressLength(arpBuffer.data(), sizeof(AU::Address));
        AU::setProtocolAddressLength(arpBuffer.data(), sizeof(std::uint32_t));
        AU::setOperation(arpBuffer.data(), AU::Operation::REQUEST);
        AU::setSenderHardwareAddress(arpBuffer.data(), d_hardwareAddress);
        AU::setSenderProtocolAddress(arpBuffer.data(), 0x0100007f);
        // skipping target hardware address
        AU::setTargetProtocolAddress(arpBuffer.data(), d_address);
        if (broadcast(hauberk::EthernetUtil::Type::ARP,
                      arpBuffer.data(),
                      arpBuffer.size())) {
            errorStream << "Failed to send ARP request\n";
            return -1;
        }
        */
      } return 0;

      default: {
        errorStream << "Unsupported data link type: " << d_datalinkType
                    << '\n';
      } return -1;
    }
}

int Duplex::send(hauberk::EthernetUtil::Type  packetType,
                 const std::uint8_t          *packetData,
                 std::size_t                  packetLength)
{
    std::vector<std::uint8_t> buffer;

    switch (d_datalinkType) {
      case DLT_NULL: {
        if (packetType != hauberk::EthernetUtil::Type::INTERNET) {
            // TBD: diagnostic?
            return -1;
        }

        typedef hauberk::LoopbackUtil LU;
        // TBD: introduce constant from LU
        buffer.resize(sizeof(LU::Family) + packetLength);
        LU::setProtocolFamily(buffer.data(), LU::Family::INTERNET);
        LU::copyPayload(buffer.data(), packetData, packetLength);

      } break;

      case DLT_EN10MB: {
        typedef hauberk::EthernetUtil EU;
        // TBD: introduce constant from EU
        buffer.resize(14 + packetLength);
        EU::setDestinationAddress(buffer.data(), d_gatewayHardwareAddress);
        EU::setSourceAddress(buffer.data(), d_hardwareAddress);
        EU::setType(buffer.data(), packetType);
        EU::copyPayload(buffer.data(), packetData, packetLength);
        // TBD: frame check sequence?
      } break;

      default:
        return -1;
    }

    if (pcap_sendpacket(d_pcapHandle.get(), buffer.data(), buffer.size())) {
        return -1;
    }

    return 0;
}

int Duplex::broadcast(hauberk::EthernetUtil::Type  packetType,
                      const std::uint8_t          *packetData,
                      std::size_t                  packetLength)
{
    std::vector<std::uint8_t> buffer;

    switch (d_datalinkType) {
      case DLT_NULL:
        // Broadcast is only supported on ethernet
        return -1;

      case DLT_EN10MB: {
        typedef hauberk::EthernetUtil EU;

        buffer.resize(14 + packetLength);
        EU::setDestinationAddress(buffer.data(),
                                  {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }});
        EU::setSourceAddress(buffer.data(), d_hardwareAddress);
        EU::setType(buffer.data(), packetType);
        EU::copyPayload(buffer.data(), packetData, packetLength);
      } break;

      default:
        return -1;
    }

    if (pcap_sendpacket(d_pcapHandle.get(), buffer.data(), buffer.size())) {
        return -1;
    }

    return 0;
}

// ACCESSORS
std::uint32_t Duplex::address() const
{
    return d_address;
}

}  // close namespace 'trammel'

