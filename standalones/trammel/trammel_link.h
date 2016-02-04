// trammel_link.h
#ifndef TRAMMEL_LINK
#define TRAMMEL_LINK

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

                                 // ==========
                                 // class Link
                                 // ==========

class Link {
  public:
    // TYPES
    enum class PacketType {
        Internet,
        ARP,
    };

    typedef std::function<int (PacketType          type,
                               const std::uint8_t *data,
                               std::size_t         length)> PacketHandler;

    enum class Type {
        Loopback,
        Ethernet,
    };

    typedef std::array<std::uint8_t, 6> Address;

  private:
    // PRIVATE TYPES
    typedef std::unique_ptr<struct pcap, void (*)(struct pcap *)> Pcap;

    // DATA
    Address                d_address;
    Pcap                   d_pcapHandle;
    PacketHandler          d_packetHandler;
    Type                   d_type;
    maxwell::Queue::Handle d_readHandle;

    // MODIFIERS
    int read(PacketType          *type,
             const std::uint8_t **data,
             std::size_t         *length);
        // TBD: contract

    int packetsReady(std::uintptr_t);
        // TBD: contract

  public:
    // CREATORS
    explicit Link();
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream&         errorStream,
             const maxwell::Queue& queue,
             const std::string&    interfaceName,
             const PacketHandler&  packetHandler,
             int                   timeoutMilliseconds,
             int                   snapshotLength,
             int                   nonblock);
        // TBD: contract

    int send(PacketType          packetType,
             const std::uint8_t *data,
             std::size_t         length);
        // TBD: contract

    int broadcast(PacketType          packetType,
                  const std::uint8_t *data,
                  std::size_t         length);
        // TBD: contract

    // ACCESSORS
    Type type() const;
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
