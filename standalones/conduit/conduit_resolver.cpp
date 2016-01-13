// conduit_resolver.cpp

#include <conduit_resolver.h>

#include <hauberk_internet.h>
#include <trammel_duplex.h>

namespace conduit {

                               // --------------
                               // class Resolver
                               // --------------

// CREATORS
Resolver::Resolver(
                ArgumentParser::InterfaceAddresses::const_iterator endpoint,
                ArgumentParser::InterfaceAddresses::const_iterator endpointEnd)
: d_duplexes()
{
    for (; endpoint != endpointEnd; ++endpoint) {
        // TBD: actually set a handler
        d_duplexes.push_back(trammel::Duplex::create(endpoint->first, 0, 0));
    }
}

// MANIPULATORS
int Resolver::open(std::ostream&         errorStream,
                   int                   timeoutMilliseconds,
                   int                   snapshotLength,
                   int                   nonblock,
                   const maxwell::Queue& queue)
{
    for (auto& duplex: d_duplexes) {
        if (duplex.open(errorStream,
                        timeoutMilliseconds,
                        snapshotLength,
                        nonblock,
                        queue)) {
            return -1;
        }
    }
    return 0;
}

int Resolver::resolve(const hauberk::Internet&  internet,
                      ResolutionHandler         handler,
                      void                     *userData)
{
    return 0;
}

}  // close namespace 'conduit'

