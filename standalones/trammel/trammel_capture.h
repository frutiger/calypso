// trammel_capture.h
#ifndef TRAMMEL_CAPTURE
#define TRAMMEL_CAPTURE

#ifndef INCLUDED_HAUBERK_LOOPBACK
#include <hauberk_loopback.h>
#endif

#ifndef INCLUDED_HAUBERK_ETHERNET
#include <hauberk_ethernet.h>
#endif

#ifndef INCLUDED_MAXWELL_EVENTHANDLER
#include <maxwell_eventhandler.h>
#endif

#ifndef INCLUDED_CHRONO
#define INCLUDED_CHRONO
#include <chrono>
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
struct pcap_pkthdr;

namespace hauberk { class Internet; }
namespace maxwell { class Queue;    }

namespace trammel {

                               // =============
                               // class Capture
                               // =============

class Capture {
  private:
    // PRIVATE TYPES

                           // =====================
                           // class CaptureMetadata
                           // =====================

    class Metadata {
        // DATA
        const struct pcap_pkthdr *d_header;

      public:
        // CREATORS
        explicit Metadata();
            // TBD: contract

        explicit Metadata(const struct pcap_pkthdr *header);
            // TBD: contract

        // ACCESSORS
        std::chrono::time_point<std::chrono::system_clock> time() const;
            // TBD: contract

        std::uint32_t capturedDataLength() const;
            // TBD: contract

        std::uint32_t dataLength() const;
            // TBD: contract
    };

    // DATA
    std::string                                            d_interface;
    hauberk::Ethernet::Address                             d_hardwareAddress;
    PacketHandler                                          d_packetHandler;
    void                                                  *d_handlerUserData;
    std::unique_ptr<struct pcap, void (*)(struct pcap *)>  d_handle;
    int                                                    d_datalinkType;
    maxwell::EventHandler                                  d_eventHandler;
    std::shared_ptr<void>                                  d_readHandle;

    // PRIVATE CLASS METHODS
    static int dispatchEvent(std::uintptr_t, void *userData);

    // MODIFIERS
    int read(hauberk::Internet *internet);
        // TBD: contract

    int packetsReady();
        // TBD: contract

  public:
    // CREATORS
    Capture(const std::string&                 interface,
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
};

}  // close namespace 'trammel'

#endif
