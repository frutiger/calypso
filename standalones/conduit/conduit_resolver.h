// conduit_resolver.h
#ifndef CONDUIT_RESOLVER
#define CONDUIT_RESOLVER

#ifndef INCLUDED_CONDUIT_ARGUMENTUTIL
#include <conduit_argumentutil.h>
#endif

#ifndef INCLUDED_TRAMMEL_DUPLEX
#include <trammel_duplex.h>
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
    typedef std::function<int (const std::uint8_t *packetData,
                               std::size_t         packetLength,
                               std::size_t         duplexIndex)> Handler;

  private:
    // PRIVATE TYPES
    typedef std::vector<trammel::Duplex>              Duplexes;
    typedef std::independent_bits_engine<std::mt19937, 16, std::uint16_t>
                                                      Engine;
    typedef std::tuple<std::uint16_t, Duplexes::size_type, Handler>
                                                      Transaction;
    typedef std::unordered_map<std::uint16_t, Transaction>
                                                      Transactions;

    // DATA
    Duplexes      d_duplexes;
    Engine        d_engine;
    Transactions  d_transactions;

    // MODIFIERS
    int processPacket(hauberk::EthernetUtil::Type  type,
                      const std::uint8_t          *packetData,
                      std::size_t                  packetLength);
        // TBD: contract

  public:
    // CREATORS
    Resolver(ArgumentUtil::Duplexes::const_iterator output,
             ArgumentUtil::Duplexes::const_iterator outputEnd);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream&         errorStream,
             int                   timeoutMilliseconds,
             int                   snapshotLength,
             int                   nonblock,
             const maxwell::Queue& queue);
        // TBD: contract

    int resolve(const std::uint8_t *request,
                std::size_t         requestLength,
                const Handler&      handler);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
