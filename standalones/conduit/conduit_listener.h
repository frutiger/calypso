// conduit_listener.h
#ifndef CONDUIT_LISTENER
#define CONDUIT_LISTENER

#ifndef INCLUDED_CONDUIT_ARGUMENTPARSER
#include <conduit_argumentparser.h>
#endif

#ifndef INCLUDED_CONDUIT_RESOLVER
#include <conduit_resolver.h>
#endif

#ifndef INCLUDED_MAXWELL_EVENTHANDLER
#include <maxwell_eventhandler.h>
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

namespace hauberk { class Internet; }
namespace maxwell { class Queue;    }

namespace conduit {

                               // ==============
                               // class Listener
                               // ==============

class Listener {
    // DATA
    std::uint32_t   d_address;
    trammel::Duplex d_duplex;
    Resolver        d_resolver;

    // PRIVATE CLASS METHODS
    static int dispatchPacket(const hauberk::Internet&  internet,
                              void                     *userData);
        // TBD: contract

    // MODIFIERS
    int processDnsRequest(const hauberk::Internet& internet);
        // TBD: contract

    int processPacket(const hauberk::Internet& internet);
        // TBD: contract

  public:
    // DELETED METHODS
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    // CREATORS
    Listener(const std::string&                                 interface,
             std::uint32_t                                      address,
             ArgumentParser::InterfaceAddresses::const_iterator endpoint,
             ArgumentParser::InterfaceAddresses::const_iterator endpointEnd);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream, const maxwell::Queue& queue);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
