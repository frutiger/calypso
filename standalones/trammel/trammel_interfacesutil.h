// trammel_interfacesutil.h
#ifndef TRAMMEL_INTERFACESUTIL
#define TRAMMEL_INTERFACESUTIL

#ifndef INCLUDED_HAUBERK_ETHERNETUTIL
#include <hauberk_ethernetutil.h>
#endif

#ifndef INCLUDED_TRAMMEL_LIST
#include <trammel_list.h>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

struct pcap_if;

namespace trammel {

class Interface;

                            // ====================
                            // class InterfacesUtil
                            // ====================

struct InterfacesUtil {
    // TYPES
    typedef List<const struct pcap_if, Interface> Interfaces;

    // CLASS METHODS
    static int list(Interfaces *interfaces, std::string *error = 0);
        // TBD: contract

    static int fromAddress(std::string                    *interfaceName,
                           hauberk::EthernetUtil::Address *hardwareAddress,
                           std::uint32_t                   target,
                           const Interfaces&               interfaces);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
