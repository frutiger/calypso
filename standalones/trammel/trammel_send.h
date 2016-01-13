// trammel_send.h
#ifndef TRAMMEL_SEND
#define TRAMMEL_SEND

#ifndef INCLUDED_HAUBERK_ETHERNET
#include <hauberk_ethernet.h>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

struct pcap;

namespace hauberk { class Internet; }

namespace trammel {

                                 // ==========
                                 // class Send
                                 // ==========

class Send {
    // DATA
    std::string                                           d_interface;
    hauberk::Ethernet::Address                            d_hardwareAddress;
    int                                                   d_datalinkType;
    std::unique_ptr<struct pcap, void (*)(struct pcap *)> d_handle;

  public:
    // CREATORS
    Send(const std::string&                interface,
         const hauberk::Ethernet::Address& hardwareAddress);
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream);
        // TBD: contract

    int write(const hauberk::Internet& internet);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
