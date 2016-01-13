// trammel_interface.cpp

#include <trammel_interface.h>

#include <pcap/pcap.h>

namespace trammel {

                              // ---------------
                              // class Interface
                              // ---------------

// CREATORS
Interface::Interface(const std::shared_ptr<const struct pcap_if>& interface)
: d_interface(interface)
{
}

// ACCESSORS
trammel::List<const struct pcap_addr, trammel::Address>
Interface::addresses() const
{
    std::shared_ptr<const struct pcap_addr> head(d_interface,
                                                 d_interface->addresses);
    return List<const struct pcap_addr, Address>(head);
}

const char *Interface::description() const
{
    return d_interface->description;
}

bool Interface::isLoopback() const
{
    return d_interface->flags & PCAP_IF_LOOPBACK;
}

const char *Interface::name() const
{
    return d_interface->name;
}

}  // close namespace 'trammel'

