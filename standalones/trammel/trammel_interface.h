// trammel_interface.h
#ifndef TRAMMEL_INTERFACE
#define TRAMMEL_INTERFACE

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif

#ifndef INCLUDED_TRAMMEL_LIST
#include <trammel_list.h>
#endif

struct pcap_addr;
struct pcap_if;

namespace trammel {

class Address;

                              // ===============
                              // class Interface
                              // ===============

class Interface {
    // DATA
    std::shared_ptr<const struct pcap_if> d_interface;

  public:
    // CREATORS
    explicit Interface(const std::shared_ptr<const struct pcap_if>& interface);
        // TBD: contract

    // ACCESSORS
    List<const struct pcap_addr, Address> addresses() const;
        // TBD: contract

    const char *description() const;
        // TBD: contract

    bool isLoopback() const;
        // TBD: contract

    const char *name() const;
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
