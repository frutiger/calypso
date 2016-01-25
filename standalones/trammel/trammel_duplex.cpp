// trammel_duplex.cpp

#include <trammel_duplex.h>

#include <hauberk_arputil.h>
#include <hauberk_ethernetutil.h>
#include <hauberk_internetutil.h>
#include <hauberk_loopbackutil.h>

#include <cassert>
#include <random>

#include <pcap/pcap.h>

#include <iostream>
#include <iterator>

namespace {
static const hauberk::EthernetUtil::Address BROADCAST_ADDRESS =
                                      {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};
}  // close unnamed namespace

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
            if (destinationAddress != d_hardwareAddress &&
                                   destinationAddress != ::BROADCAST_ADDRESS) {
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
        switch (packetType) {
          case hauberk::EthernetUtil::Type::ARP: {
            typedef hauberk::ArpUtil AU;

            const std::uint8_t *arp = packetData;
            //std::cout << "ARP request!\n";
            if (AU::targetProtocolAddress(arp) == d_source) {
                std::cout << "self ARP request!\n";
            }
          } break;

          case hauberk::EthernetUtil::Type::INTERNET: {
            typedef hauberk::InternetUtil IU;

            const std::uint8_t *internet = packetData;
            if (IU::destinationAddress(internet) != d_source) {
                continue;
            }

            if (d_packetHandler(packetType, packetData, packetLength)) {
                return -1;
            }
          } break;

          default:
            continue;
        }
    }
    return 0;
}

// CREATORS
Duplex::Duplex(const std::string&   interfaceName,
               std::uint32_t        source,
               std::uint32_t        destination,
               const PacketHandler& packetHandler)
: d_interfaceName(interfaceName)
, d_source(source)
, d_destination(destination)
, d_packetHandler(packetHandler)
, d_hardwareAddress()
, d_pcapHandle(0, &pcap_close)
, d_datalinkType()
, d_readHandle()
{
    typedef std::independent_bits_engine<std::mt19937, 8, std::uint8_t> Engine;

    d_hardwareAddress[0] = 0;
    Engine engine((std::mt19937(std::random_device()())));
    std::generate(d_hardwareAddress.begin() + 1,
                  d_hardwareAddress.end(),
                  engine);
    d_hardwareAddress = {{0xac, 0xbc, 0x32, 0xcd, 0x3a, 0x6f}};
}

// MANIPULATORS
int Duplex::open(std::ostream&         errorStream,
                 int                   timeoutMilliseconds,
                 int                   snapshotLength,
                 int                   nonblock,
                 const maxwell::Queue& queue)
{
    char         createErrorBuffer[PCAP_ERRBUF_SIZE];
    struct pcap *capture = pcap_create(d_interfaceName.c_str(),
                                       createErrorBuffer);
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
        // skipping sender protocol receiver
        // skipping target hardware receiver
        AU::setTargetProtocolAddress(arpBuffer.data(), d_source);
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
        typedef hauberk::InternetUtil IU;

        // TBD: introduce constant from LU
        buffer.resize(sizeof(LU::Family) + packetLength);
        LU::setProtocolFamily(buffer.data(), LU::Family::INTERNET);
        LU::copyPayload(buffer.data(), packetData, packetLength);

        std::uint8_t *internet = LU::payload(buffer.data());
        IU::setSourceAddress(internet, d_source);
        IU::setDestinationAddress(internet, d_destination);
        IU::updateChecksum(internet);
        // TBD: frame check sequence?
      } break;

      case DLT_EN10MB: {
        typedef hauberk::EthernetUtil EU;
        typedef hauberk::InternetUtil IU;

        // TBD: introduce constant from EU
        buffer.resize(14 + packetLength);
        // TBD: ARP out the hardware receiver
        EU::setDestinationAddress(buffer.data(),
                                  {{ 0x00, 0xF7, 0x6F, 0xD6, 0x2F, 0x28 }});
        EU::setSourceAddress(buffer.data(), d_hardwareAddress);
        EU::setType(buffer.data(), packetType);
        EU::copyPayload(buffer.data(), packetData, packetLength);

        std::uint8_t *internet = EU::payload(buffer.data());
        IU::setSourceAddress(internet, d_source);
        IU::setDestinationAddress(internet, d_destination);
        IU::updateChecksum(internet);
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
        EU::setDestinationAddress(buffer.data(), ::BROADCAST_ADDRESS);
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

}  // close namespace 'trammel'

