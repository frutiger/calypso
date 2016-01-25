// conduit_listener.h
#ifndef CONDUIT_LISTENER
#define CONDUIT_LISTENER

#ifndef INCLUDED_CONDUIT_ARGUMENTUTIL
#include <conduit_argumentutil.h>
#endif

#ifndef INCLUDED_CONDUIT_RESOLVER
#include <conduit_resolver.h>
#endif

#ifndef INCLUDED_TRAMMEL_DUPLEX
#include <trammel_duplex.h>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

namespace maxwell { class Queue;    }

namespace conduit {

                               // ==============
                               // class Listener
                               // ==============

class Listener {
    // DATA
    trammel::Duplex d_input;
    Resolver        d_resolver;

    // MODIFIERS
    int processDnsResponse(const std::uint8_t *packetData,
                           std::size_t         packetLength,
                           std::size_t         duplexIndex);
        // TBD: contract

    int processDnsRequest(const std::uint8_t *request,
                          std::size_t         requestLength);
        // TBD: contract

    int processPacket(hauberk::EthernetUtil::Type  type,
                      const std::uint8_t          *packetData,
                      std::size_t                  packetLength);
        // TBD: contract

  public:
    // DELETED METHODS
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    // CREATORS
    Listener(const ArgumentUtil::Simplex&           input,
             ArgumentUtil::Duplexes::const_iterator output,
             ArgumentUtil::Duplexes::const_iterator outputEnd);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream, const maxwell::Queue& queue);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
