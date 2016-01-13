// conduit_listener.h
#ifndef CONDUIT_LISTENER
#define CONDUIT_LISTENER

#ifndef INCLUDED_MAXWELL_EVENTHANDLER
#include <maxwell_eventhandler.h>
#endif

#ifndef INCLUDED_TRAMMEL_CAPTURE
#include <trammel_capture.h>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

namespace hauberk { class Internet; }
namespace maxwell { class Queue;    }

namespace conduit {

                               // ==============
                               // class Listener
                               // ==============

class Listener {
    // DATA
    std::string                d_interface;
    std::uint32_t              d_address;
    maxwell::EventHandler      d_eventHandler;
    std::shared_ptr<void>      d_readHandle;
    trammel::Capture           d_capture;
    bool                       d_activated;
    trammel::Capture::LinkType d_linkType;

    // PRIVATE CLASS METHODS
    static int dispatchEvent(std::uintptr_t, void *userData);

    // MODIFIERS
    int processDnsRequest(const hauberk::Internet& internet);
        // TBD: contract

    int processLoopbackPacket(const uint8_t *data);
        // TBD: contract

    int processEthernetPacket(const uint8_t *data);
        // TBD: contract

    int processPacket(const trammel::CaptureMetadata *metadata,
                      const uint8_t                  *data);
        // TBD: contract

    int packetsReady();
        // TBD: contract

  public:
    // CREATORS
    Listener(const std::string& interface, std::uint32_t address);
        // TBD: contract

    // MANIPULATORS
    int activate(std::ostream& errorStream, const maxwell::Queue& queue);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
