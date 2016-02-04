// conduit_listener.h
#ifndef CONDUIT_LISTENER
#define CONDUIT_LISTENER

#ifndef INCLUDED_CONDUIT_ARGUMENTUTIL
#include <conduit_argumentutil.h>
#endif

#ifndef INCLUDED_CONDUIT_RESOLVER
#include <conduit_resolver.h>
#endif

#ifndef INCLUDED_TRAMMEL_LINK
#include <trammel_link.h>
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
    typedef std::unordered_map<std::uint32_t,
                               Resolver::Gateways::size_type> Routes;

    // DATA
    trammel::Link      d_input;
    Resolver::Gateways d_gateways;
    Resolver           d_resolver;
    Routes             d_routes;

    // MODIFIERS
    int processInputPacket(trammel::Link::PacketType  type,
                           const std::uint8_t        *data,
                           std::size_t                length);
        // TBD: contract

    int processOutputPacket(trammel::Link::PacketType  type,
                            const std::uint8_t        *data,
                            std::size_t                length);
        // TBD: contract

    void routeAdvised(std::uint32_t          destination,
                      Resolver::GatewayIndex gatewayIndex);
        // TBD: contract

  public:
    // DELETED METHODS
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    // CREATORS
    Listener();
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream&                          errorStream,
             const maxwell::Queue&                  queue,
             const ArgumentUtil::Simplex&           input,
             ArgumentUtil::Duplexes::const_iterator output,
             ArgumentUtil::Duplexes::const_iterator outputEnd);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
