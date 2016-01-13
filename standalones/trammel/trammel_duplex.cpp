// trammel_duplex.cpp

#include <trammel_duplex.h>

#include <random>

namespace trammel {

                                // ------------
                                // class Duplex
                                // ------------

// PRIVATE CLASS METHODS
int Duplex::dispatchPacket(const hauberk::Internet& internet, void *userData)
{
    return static_cast<Duplex *>(userData)->handlePacket(internet);
}

// MODIFIERS
int Duplex::handlePacket(const hauberk::Internet& internet)
{
}

// CLASS METHODS
Duplex Duplex::create(const std::string& interface)
{
    typedef std::independent_bits_engine<std::mt19937, 8, std::uint8_t> Engine;

    Engine                     engine((std::mt19937(std::random_device()())));
    hauberk::Ethernet::Address hardwareAddress;
    std::generate(hardwareAddress.begin(), hardwareAddress.end(), engine);
    return Duplex(interface, hardwareAddress);
}

// CREATORS
Duplex::Duplex(const std::string&                interface,
               const hauberk::Ethernet::Address& hardwareAddress)
: d_sender(interface, hardwareAddress)
, d_receiver(interface, hardwareAddress, &Duplex::dispatchPacket, this)
{
}

// MANIPULATORS
int Duplex::open(std::ostream&         errorStream,
                 int                   timeoutMilliseconds,
                 int                   snapshotLength,
                 int                   nonblock,
                 const maxwell::Queue& queue)
{
    if (d_sender.open(errorStream)) {
        return -1;
    }

    if (d_receiver.open(errorStream,
                        timeoutMilliseconds,
                        snapshotLength,
                        nonblock,
                        queue)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'trammel'

