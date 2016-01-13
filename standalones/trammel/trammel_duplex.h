// trammel_duplex.h
#ifndef TRAMMEL_DUPLEX
#define TRAMMEL_DUPLEX

#ifndef INCLUDED_HAUBERK_ETHERNET
#include <hauberk_ethernet.h>
#endif

#ifndef INCLUDED_MAXWELL_EVENTHANDLER
#include <maxwell_eventhandler.h>
#endif

#ifndef INCLUDED_ARRAY
#define INCLUDED_ARRAY
#include <array>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
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

namespace hauberk { class Internet; }
namespace maxwell { class Queue;    }

namespace trammel {

                                // ============
                                // class Duplex
                                // ============

class Duplex {
  public:
    // TYPES
    typedef int (*PacketHandler)(const hauberk::Internet&  internet,
                                 void                     *userData);

  private:
    // DATA
    std::string                                            d_interface;
    hauberk::Ethernet::Address                             d_hardwareAddress;
    PacketHandler                                          d_packetHandler;
    void                                                  *d_handlerUserData;
    std::unique_ptr<struct pcap, void (*)(struct pcap *)>  d_handle;
    int                                                    d_datalinkType;
    maxwell::EventHandler                                  d_eventHandler;
    std::shared_ptr<void>                                  d_readHandle;

    // PRIVATE CLASS METHOD
    static int dispatchEvent(std::uintptr_t, void *userData);

    // MODIFIERS
    int read(hauberk::Internet *internet);
        // TBD: contract

    int packetsReady();
        // TBD: contract

  public:
    // CLASS METHODS
    static Duplex create(const std::string&  interface,
                         PacketHandler       packetHandler,
                         void               *handlerUserData);
        // TBD: contract

    // CREATORS
    explicit Duplex(const std::string&                 interface,
                    const hauberk::Ethernet::Address&  hardwareAddress,
                    PacketHandler                      packetHandler,
                    void                              *handlerUserData);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream&         errorStream,
             int                   timeoutMilliseconds,
             int                   snapshotLength,
             int                   nonblock,
             const maxwell::Queue& queue);
        // TBD: contract

    int send(const hauberk::Internet& internet);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
