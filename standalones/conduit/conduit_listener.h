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

#ifndef INCLUDED_UNORDERED_MAP
#define INCLUDED_UNORDERED_MAP
#include <unordered_map>
#endif

namespace maxwell { class Queue; }

namespace conduit {

                               // ==============
                               // class Listener
                               // ==============

class Listener {
    // PRIVATE TYPES
    struct Query {
        std::size_t d_preferredGateway;
        std::size_t d_gatewaysRemaining;
    };

    typedef std::vector<std::uint32_t>                       Gateways;
    typedef std::unordered_map<std::string, Query>           Queries;
    typedef std::unordered_map<std::uint32_t, std::uint32_t> Routes;

    // DATA
    trammel::Duplex d_input;
    Gateways        d_gateways;
    Resolver        d_resolver;
    Routes          d_routes;

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
