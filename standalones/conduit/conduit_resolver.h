// conduit_resolver.h
#ifndef CONDUIT_RESOLVER
#define CONDUIT_RESOLVER

#ifndef INCLUDED_CONDUIT_ARGUMENTPARSER
#include <conduit_argumentparser.h>
#endif

#ifndef INCLUDED_TRAMMEL_DUPLEX
#include <trammel_duplex.h>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_UTILITY
#define INCLUDED_UTILITY
#include <utility>
#endif

#ifndef INCLUDED_VECTOR
#define INCLUDED_VECTOR
#include <vector>
#endif

namespace hauberk { class Internet; }

namespace conduit {

                               // ==============
                               // class Resolver
                               // ==============

class Resolver {
    // PRIVATE TYPES
    typedef std::vector<trammel::Duplex> Duplexes;

    // DATA
    Duplexes d_duplexes;

  public:
    // TYPES
    typedef void (*ResolutionHandler)(const hauberk::Internet&  request,
                                      std::uint32_t             gateway,
                                      const hauberk::Internet&  response,
                                      void                     *userData);

    // CREATORS
    Resolver(ArgumentParser::InterfaceAddresses::const_iterator endpoint,
             ArgumentParser::InterfaceAddresses::const_iterator endpointEnd);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream);
        // TBD: contract

    int resolve(const hauberk::Internet&  internet,
                ResolutionHandler         handler,
                void                     *userData);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
