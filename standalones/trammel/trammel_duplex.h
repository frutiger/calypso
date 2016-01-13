// trammel_duplex.h
#ifndef TRAMMEL_DUPLEX
#define TRAMMEL_DUPLEX

#ifndef INCLUDED_TRAMMEL_CAPTURE
#include <trammel_capture.h>
#endif

#ifndef INCLUDED_TRAMMEL_SEND
#include <trammel_send.h>
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

namespace hauberk { class Internet; }
namespace maxwell { class Queue;    }

namespace trammel {

                                // ============
                                // class Duplex
                                // ============

class Duplex {
  private:
    // DATA
    Send    d_sender;
    Capture d_receiver;

    // PRIVATE CLASS METHODS
    static int dispatchPacket(const hauberk::Internet&  internet,
                              void                     *userData);
        // TBD: contract

    // MODIFIERS
    int handlePacket(const hauberk::Internet& internet);
        // TBD: contract

  public:
    // CLASS METHODS
    static Duplex create(const std::string& interface);
        // TBD: contract

    // CREATORS
    explicit Duplex(const std::string&                interface,
                    const hauberk::Ethernet::Address& hardwareAddress);
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
