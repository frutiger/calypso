// conduit_argumentutil.h
#ifndef CONDUIT_ARGUMENTUTIL
#define CONDUIT_ARGUMENTUTIL

#ifndef INCLUDED_TRAMMEL_LIST
#include <trammel_list.h>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

#ifndef INCLUDED_VECTOR
#define INCLUDED_VECTOR
#include <vector>
#endif

struct pcap_if;

namespace trammel { class Interface; }

namespace conduit {

                             // ==================
                             // class ArgumentUtil
                             // ==================

struct ArgumentUtil {
    // TYPES
    struct Simplex {
        std::string   d_interfaceName;
        std::uint32_t d_source;
    };

    struct Duplex {
        std::string d_interfaceName;
        struct Tunnel {
            std::uint32_t d_source;
            std::uint32_t d_destination;
        } d_tunnel;
    };

    typedef trammel::List<const struct pcap_if, trammel::Interface> Interfaces;
    typedef std::vector<Simplex>                                    Simplexes;
    typedef std::vector<Duplex>                                     Duplexes;

    // CLASS METHODS
    static int toSimplex(std::ostream&       errorStream,
                         Simplex            *result,
                         const Interfaces&   interfaces,
                         const std::string&  argument);

    static int toNamedSimplex(std::ostream&       errorStream,
                              Simplex            *result,
                              const Interfaces&   interfaces,
                              const std::string&  argument);

    static int toDuplexTunnel(std::ostream&       errorStream,
                              Duplex::Tunnel     *result,
                              const std::string&  argument);

    static int toDuplex(std::ostream&       errorStream,
                        Duplex             *result,
                        const Interfaces&   interfaces,
                        const std::string&  argument);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
