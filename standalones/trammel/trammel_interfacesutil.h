// trammel_interfacesutil.h
#ifndef TRAMMEL_INTERFACESUTIL
#define TRAMMEL_INTERFACESUTIL

#ifndef INCLUDED_TRAMMEL_LIST
#include <trammel_list.h>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
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
    static int list(std::ostream& errorStream, Interfaces *interfaces);
        // TBD: contract

    static int fromAddress(std::string       *interfaceName,
                           std::uint32_t      target,
                           const Interfaces&  interfaces);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
