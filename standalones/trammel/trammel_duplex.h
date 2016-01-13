// trammel_duplex.h
#ifndef TRAMMEL_DUPLEX
#define TRAMMEL_DUPLEX

#ifndef INCLUDED_TRAMMEL_CAPTURE
#include <trammel_capture.h>
#endif

#ifndef INCLUDED_TRAMMEL_SEND
#include <trammel_send.h>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

#ifndef INCLUDED_TUPLE
#define INCLUDED_TUPLE
#include <tuple>
#endif

namespace hauberk { class Loopback; }
namespace hauberk { class Ethernet; }

namespace trammel {

                                // ============
                                // class Duplex
                                // ============

class Duplex {
  public:
    // TYPES
    typedef std::tuple<hauberk::Loopback, hauberk::Ethernet> Packet;
    typedef void (*PacketHandler)(const Packet& packet, void *userData);

  private:
    // DATA
    std::string   d_interface;
    PacketHandler d_handler;
    Send          d_sender;
    Capture       d_receiver;

  public:
    // CREATORS
    explicit Duplex(const std::string& interface, PacketHandler handler);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
