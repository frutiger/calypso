// conduit_resolver.h
#ifndef CONDUIT_RESOLVER
#define CONDUIT_RESOLVER

#include <conduit_argumentparser.h>
#include <trammel_capture.h>
#include <trammel_send.h>
#include <cstdint>
#include <utility>
#include <vector>

namespace hauberk { class Internet; }

namespace conduit {

                               // ==============
                               // class Resolver
                               // ==============

class Resolver {
    // DATA
    struct EndpointDuplex {
        // DATA
        std::uint32_t    d_address;
        trammel::Send    d_sender;
        trammel::Capture d_receiver;
    };
    typedef std::vector<EndpointDuplex> EndpointDuplexes;

    ArgumentParser::InterfaceAddresses d_interfaceAddresses;
    EndpointDuplexes                   d_endpointDuplexes;

  public:
    // DELETED METHODS
    Resolver(const Resolver&)             = delete;
    Resolver(const Resolver&&)            = delete;
    Resolver& operator=(const Resolver&)  = delete;
    Resolver& operator=(const Resolver&&) = delete;

    // CREATORS
    Resolver(ArgumentParser::InterfaceAddresses::const_iterator endpoint,
             ArgumentParser::InterfaceAddresses::const_iterator endpointEnd);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream);
        // TBD: contract

    int resolve(const hauberk::Internet& internet);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
