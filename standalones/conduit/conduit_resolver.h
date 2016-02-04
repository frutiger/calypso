// conduit_resolver.h
#ifndef CONDUIT_RESOLVER
#define CONDUIT_RESOLVER

#ifndef INCLUDED_CONDUIT_ARGUMENTUTIL
#include <conduit_argumentutil.h>
#endif

#ifndef INCLUDED_TRAMMEL_LINK
#include <trammel_link.h>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_FUNCTIONAL
#define INCLUDED_FUNCTIONAL
#include <functional>
#endif

#ifndef INCLUDED_RANDOM
#define INCLUDED_RANDOM
#include <random>
#endif

#ifndef INCLUDED_SET
#define INCLUDED_SET
#include <set>
#endif

#ifndef INCLUDED_UNORDERED_MAP
#define INCLUDED_UNORDERED_MAP
#include <unordered_map>
#endif

#ifndef INCLUDED_VECTOR
#define INCLUDED_VECTOR
#include <vector>
#endif

namespace conduit {

                               // ==============
                               // class Resolver
                               // ==============

class Resolver {
  public:
    // TYPES
    struct Gateway {
        std::uint32_t d_address;
        trammel::Link d_link;
    };
    typedef std::vector<Gateway> Gateways;
    typedef Gateways::size_type  GatewayIndex;
    typedef std::function<void (std::uint32_t destination,
                                GatewayIndex  gatewayIndex)> RouteAdvisory;

  private:
    // PRIVATE TYPES
    typedef std::independent_bits_engine<std::mt19937,
                                         16,
                                         std::uint16_t> TransactionIdGenerator;

    struct Query {
        GatewayIndex d_preferredGatewayIndex;
        GatewayIndex d_gatewaysRemaining;
        bool operator<(const Query& rhs);
    };
    typedef std::multiset<Query> Queries;

    struct SubQuery {
        Queries::iterator d_query;
        std::uint16_t     d_originalTransactionId;
        GatewayIndex      d_gatewayIndex;
    };
    typedef std::unordered_map<std::uint16_t, SubQuery> SubQueries;

    // DATA
    RouteAdvisory          d_adviseRoute;
    TransactionIdGenerator d_transactionIdGenerator;
    Queries                d_queries;
    SubQueries             d_subQueries;

  public:
    // CLASS METHODS
    static bool isNameResolution(trammel::Link::PacketType  type,
                                 const std::uint8_t        *data);
        // TBD: contract

    // CREATORS
    explicit Resolver(const RouteAdvisory& adviseRoute);
        // TBD: contract

    // MANIPULATORS
    int processInputPacket(const Gateways::iterator&  gatewayBegin,
                           Gateways::size_type        numGateways,
                           const std::uint8_t        *data,
                           std::size_t                length);
        // TBD: contract

    void processOutputPacket(std::vector<std::uint8_t> *newPacket,
                             const std::uint8_t        *data,
                             std::size_t                length);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
