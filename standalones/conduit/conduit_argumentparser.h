// conduit_argumentparser.h

#ifndef CONDUIT_ARGUMENTPARSER
#define CONDUIT_ARGUMENTPARSER

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

namespace conduit {

                            // ====================
                            // class ArgumentParser
                            // ====================

struct ArgumentParser {
    // TYPES
    typedef std::pair<std::string, std::uint32_t> ArgumentAddress;
    typedef std::vector<ArgumentAddress>          ArgumentAddresses;
    typedef std::pair<std::string, std::uint32_t> InterfaceAddress;
    typedef std::vector<InterfaceAddress>         InterfaceAddresses;

    // CLASS METHODS
    static int toAddresses(std::ostream&                   errorStream,
                           ArgumentAddresses              *result,
                           const std::string&              programName,
                           const std::vector<std::string>  arguments);
        // TBD: contract

    static int toInterfaces(std::ostream&                   errorStream,
                            InterfaceAddresses             *result,
                            const std::string&              programName,
                            const std::vector<std::string>  arguments);
        // TBD: contract

    static std::ostream& usage(std::ostream&      stream,
                               const std::string& programName);
        // TBD: contract
};

}  // close namespace 'conduit'

#endif
