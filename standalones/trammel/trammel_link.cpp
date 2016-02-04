// trammel_link.cpp

#include <trammel_link.h>

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

                                 // ----------
                                 // class Link
                                 // ----------

// MODIFIERS
int Link::read(PacketType          *type,
               const std::uint8_t **data,
               std::size_t         *length)
{
    while (true) {
        struct pcap_pkthdr *header;
        const uint8_t      *captured;
        if (pcap_next_ex(d_pcapHandle.get(), &header, &captured) != 1) {
            return -1;
        }

        // TBD: better error handling
        assert(header->caplen == header->len);

        switch (d_type) {
          case Type::Loopback: {
            typedef hauberk::LoopbackUtil LU;

            switch (LU::Family(LU::protocolFamily(*data))) {
              case LU::Family::INTERNET: {
                *type = PacketType::Internet;
              } break;

              default:
                continue;
            }

            *data   = LU::payload(captured);
            *length = header->caplen - sizeof(LU::Family);
          } break;

          case Type::Ethernet: {
            typedef hauberk::EthernetUtil EU;

            EU::Address destinationAddress;
            EU::destinationAddress(&destinationAddress, *data);
            if (destinationAddress != d_address &&
                                   destinationAddress != ::BROADCAST_ADDRESS) {
                // TBD: install BPF filter programs instead of checking the
                // packet in user space
                continue;
            }

            switch (EU::Type(EU::type(*data))) {
              case EU::Type::Internet: {
                *type = PacketType::Internet;
              } break;

              default:
                continue;
            }

            *data   = EU::payload(captured);
            *length = header->caplen - 14;
          } break;
        }

        return 0;
    }
}

int Link::packetsReady(std::uintptr_t)
{
    PacketType          type;
    const std::uint8_t *data;
    std::size_t         length;
    while (!read(&type, &data, &length)) {
        if (d_packetHandler(type, data, length)) {
            return -1;
        }
    }
    return 0;
}

// CREATORS
Link::Link()
: d_address()
, d_pcapHandle(0, &pcap_close)
, d_packetHandler()
, d_type()
, d_readHandle()
{
    typedef std::independent_bits_engine<std::mt19937, 8, std::uint8_t> Engine;
    Engine engine((std::mt19937(std::random_device()())));
    d_address[0] = 0;
    std::generate(d_address.begin() + 1, d_address.end(), engine);
}

// MANIPULATORS
int Link::open(std::ostream&         errorStream,
               const maxwell::Queue& queue,
               const std::string&    interfaceName,
               const PacketHandler&  packetHandler,
               int                   timeoutMilliseconds,
               int                   snapshotLength,
               int                   nonblock)
{
    char         createErrorBuffer[PCAP_ERRBUF_SIZE];
    d_pcapHandle.reset(pcap_create(interfaceName.c_str(), createErrorBuffer));
    if (!d_pcapHandle) {
        errorStream << "Failed to create pcap: " << createErrorBuffer;
        return -1;
    }

    d_packetHandler = packetHandler;

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

    switch (pcap_datalink(d_pcapHandle.get())) {
      case DLT_NULL: {
        d_type = Type::Loopback;
      } break;

      case DLT_EN10MB: {
        d_type = Type::Ethernet;
      } break;

      default: {
        errorStream << "Unsupported link type: "
                    << pcap_datalink(d_pcapHandle.get()) << '\n';
      } return -1;
    }

    int descriptor = pcap_get_selectable_fd(d_pcapHandle.get());
    if (descriptor == -1) {
        errorStream << "Failed to get selectable file descriptor";
        return -1;
    }

    auto handler = std::make_unique<maxwell::Queue::Handler>(
                                             std::bind(&Link::packetsReady,
                                                       this,
                                                       std::placeholders::_1));
    if (queue.setReadHandler(errorStream,
                             &d_readHandle,
                             &handler,
                             static_cast<std::uintptr_t>(descriptor))) {
        return -1;
    }

    return 0;
}

int Link::send(PacketType          packetType,
               const std::uint8_t *data,
               std::size_t         length)
{
    std::vector<std::uint8_t> packet;

    switch (d_type) {
      case Type::Loopback: {
        typedef hauberk::LoopbackUtil LU;

        switch (packetType) {
          case PacketType::Internet: {
            // TBD: introduce constant from LU
            packet.resize(sizeof(LU::Family) + length);
            LU::setProtocolFamily(packet.data(), LU::Family::INTERNET);
            LU::copyPayload(packet.data(), data, length);
          } break;

          default:
            return -1;
        }
      } break;

      case Type::Ethernet: {
        typedef hauberk::EthernetUtil EU;

        // TBD: introduce constant from EU
        packet.resize(14 + length);

        // TBD: ARP out the hardware receiver
        EU::setDestinationAddress(packet.data(),
                                  {{ 0x00, 0xF7, 0x6F, 0xD6, 0x2F, 0x28 }});
        EU::setSourceAddress(packet.data(), d_address);

        switch (packetType) {
          case PacketType::Internet: {
            EU::setType(packet.data(), EU::Type::Internet);
          } break;

          default:
            return -1;
        }

        EU::copyPayload(packet.data(), data, length);
        // TBD: frame check sequence?
      } break;

      default:
        return -1;
    }

    if (pcap_sendpacket(d_pcapHandle.get(), packet.data(), packet.size())) {
        return -1;
    }

    return 0;
}

int Link::broadcast(PacketType          packetType,
                    const std::uint8_t *data,
                    std::size_t         length)
{
    std::vector<std::uint8_t> packet;

    switch (d_type) {
      case Type::Loopback:
        // Broadcast is only supported on ethernet
        return -1;

      case Type::Ethernet: {
        typedef hauberk::EthernetUtil EU;

        // TBD: introduce constant from EU
        packet.resize(14 + length);

        EU::setDestinationAddress(packet.data(), ::BROADCAST_ADDRESS);
        EU::setSourceAddress(packet.data(), d_address);
        switch (packetType) {
          case PacketType::Internet: {
            EU::setType(packet.data(), EU::Type::Internet);
          } break;

          default:
            return -1;
        }

        EU::copyPayload(packet.data(), data, length);
        // TBD: frame check sequence?
      } break;

      default:
        return -1;
    }

    if (pcap_sendpacket(d_pcapHandle.get(), packet.data(), packet.size())) {
        return -1;
    }

    return 0;
}

}  // close namespace 'trammel'

