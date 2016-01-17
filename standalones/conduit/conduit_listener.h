// conduit_listener.h
#ifndef CONDUIT_LISTENER
#define CONDUIT_LISTENER

#ifndef INCLUDED_CONDUIT_ARGUMENTPARSER
#include <conduit_argumentparser.h>
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
    trammel::Duplex               d_duplex;
    Resolver                      d_resolver;

    // MODIFIERS
    int processDnsResponse(const std::uint8_t *packetData,
                           std::size_t         packetLength,
                           std::uint32_t       gateway);
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
    Listener(
            const ArgumentParser::InterfaceAddresses::value_type& listener,
            ArgumentParser::InterfaceAddresses::const_iterator    endpoint,
            ArgumentParser::InterfaceAddresses::const_iterator    endpointEnd);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream, const maxwell::Queue& queue);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
