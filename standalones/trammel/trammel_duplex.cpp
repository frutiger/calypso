// trammel_duplex.cpp

#include <trammel_duplex.h>

#include <random>

namespace trammel {

                                // ------------
                                // class Duplex
                                // ------------

// CREATORS
Duplex::Duplex(const std::string& interface, PacketHandler handler)
: d_interface(interface)
, d_handler(handler)
, d_sender()
, d_receiver()
{
}

// MANIPULATORS
int Duplex::open(std::ostream& errorStream)
{
    if (d_sender.open(errorStream, d_interface)) {
        return -1;
    }

    if (d_receiver.open(errorStream, d_interface)) {
        return -1;
    }

    // TBD: Use 'ARP' to discover unique MAC address on 'd_interface'
    std::independent_bits_engine<std::mt19937, 8, std::uint8_t> engine(
                                           std::mt19937(std::random_device()));

    return 0;
}

}  // close namespace 'trammel'

