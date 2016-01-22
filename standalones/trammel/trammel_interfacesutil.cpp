// trammel_interfacesutil.cpp

#include <trammel_address.h>
#include <trammel_list.h>
#include <trammel_interface.h>
#include <trammel_interfacesutil.h>

#include <hauberk_socketaddress.h>

#include <pcap/pcap.h>

namespace trammel {

                            // --------------------
                            // class InterfacesUtil
                            // --------------------

// CLASS METHODS
int InterfacesUtil::list(std::ostream& errorStream, Interfaces *interfaces)
{
    pcap_if_t *device;
    char       errorBuffer[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&device, errorBuffer)) {
        errorStream << "Failed to list interfaces: " << errorBuffer << "\n\n";
        return -1;
    }

    std::shared_ptr<pcap_if_t> head(device, &pcap_freealldevs);
    *interfaces = List<const struct pcap_if, Interface>(head);
    return 0;
}

int InterfacesUtil::fromAddress(std::string       *interfaceName,
                                std::uint32_t      target,
                                const Interfaces&  interfaces)
{
    using Interfaces = List<const struct pcap_if,   Interface>;
    using Addresses  = List<const struct pcap_addr, Address>;

    for (Interfaces::iterator interface  = interfaces.begin(),
                              end        = Interfaces::iterator();
                              interface != end;
                            ++interface) {
        Addresses addresses            = interface->addresses();
        for (Addresses::iterator address  = interface->addresses().begin(),
                                 end      = Addresses::iterator();
                                 address != end;
                               ++address) {
            hauberk::SocketAddress sendAddress = address->sendAddress();
            if (sendAddress.isInternet() &&
                                         sendAddress.theInternet() == target) {
                interfaceName->assign(interface->name());
                return 0;
            }
        }
    }
    return -1;
}

}  // close namespace 'trammel'

