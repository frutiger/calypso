// conduit_resolver.cpp

#include <conduit_resolver.h>

#include <hauberk_internet.h>

namespace conduit {

                               // --------------
                               // class Resolver
                               // --------------

// CREATORS
Resolver::Resolver(
                ArgumentParser::InterfaceAddresses::const_iterator endpoint,
                ArgumentParser::InterfaceAddresses::const_iterator endpointEnd)
: d_interfaceAddresses(endpoint, endpointEnd)
, d_endpointDuplexes()
{
}

// MANIPULATORS
int Resolver::open(std::ostream& errorStream)
{
    for (auto interfaceAddress: d_interfaceAddresses) {
        EndpointDuplex endpointDuplex;
        endpointDuplex.d_address = interfaceAddress.second;
        if (endpointDuplex.d_sender.open(errorStream,
                                         interfaceAddress.first)) {
            return -1;
        }
        if (endpointDuplex.d_receiver.open(errorStream,
                                           interfaceAddress.first)) {
            return -1;
        }
        d_endpointDuplexes.push_back(std::move(endpointDuplex));
    }
    return 0;
}

int Resolver::resolve(const hauberk::Internet&  internet,
                      ResolutionHandler         handler,
                      void                     *userData)
{
    for (EndpointDuplexes::size_type i = 0;
                                     i < d_endpointDuplexes.size();
                                   ++i) {
        //d_endpointDuplexes[i].d_sender
    }
}

}  // close namespace 'conduit'

