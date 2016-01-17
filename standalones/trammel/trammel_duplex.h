// trammel_duplex.h
#ifndef TRAMMEL_DUPLEX
#define TRAMMEL_DUPLEX

#ifndef INCLUDED_HAUBERK_ETHERNETUTIL
#include <hauberk_ethernetutil.h>
#endif

#ifndef INCLUDED_MAXWELL_QUEUE
#include <maxwell_queue.h>
#endif

#ifndef INCLUDED_ARRAY
#define INCLUDED_ARRAY
#include <array>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_FUNCTIONAL
#define INCLUDED_FUNCTIONAL
#include <functional>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

struct pcap;

namespace trammel {

                                // ============
                                // class Duplex
                                // ============

class Duplex {
  public:
    // TYPES
    typedef std::function<int (hauberk::EthernetUtil::Type  packetType,
                               const std::uint8_t          *packetData,
                               std::size_t                  packetLength)>
                                                                 PacketHandler;

  private:
    // PRIVATE TYPES
    typedef std::unique_ptr<struct pcap, void (*)(struct pcap *)> Pcap;

    // DATA
    std::string                    d_interface;
    std::uint32_t                  d_address;
    hauberk::EthernetUtil::Address d_hardwareAddress;
    PacketHandler                  d_packetHandler;
    Pcap                           d_pcapHandle;
    int                            d_datalinkType;
    maxwell::Queue::Handle         d_readHandle;
    hauberk::EthernetUtil::Address d_gatewayHardwareAddress;

    // MODIFIERS
    int read(hauberk::EthernetUtil::Type  *type,
             const std::uint8_t          **packetData,
             std::size_t                  *packetLength);
        // TBD: contract

    int packetsReady(std::uintptr_t);
        // TBD: contract

  public:
    // CREATORS
    Duplex(const std::string&                    interface,
           const hauberk::EthernetUtil::Address& gatewayHardwareAddress,
           std::uint32_t                         address,
           const PacketHandler&                  packetHandler);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream&         errorStream,
             int                   timeoutMilliseconds,
             int                   snapshotLength,
             int                   nonblock,
             const maxwell::Queue& queue);
        // TBD: contract

    int send(hauberk::EthernetUtil::Type  packetType,
             const std::uint8_t          *packetData,
             std::size_t                  packetLength);
        // TBD: contract

    int broadcast(hauberk::EthernetUtil::Type  packetType,
                  const std::uint8_t          *packetData,
                  std::size_t                  packetLength);
        // TBD: contract

    // ACCESSORS
    std::uint32_t address() const;
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
