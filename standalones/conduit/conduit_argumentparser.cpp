// conduit_argumentparser.cpp

#include <conduit_argumentparser.h>

#include <hauberk_internetaddressutil.h>
#include <trammel_address.h>
#include <trammel_interface.h>
#include <trammel_interfacesutil.h>
#include <trammel_list.h>

namespace conduit {

                            // --------------------
                            // class ArgumentParser
                            // --------------------

// CLASS METHODS
int ArgumentParser::toAddresses(std::ostream&                   errorStream,
                                ArgumentAddresses              *result,
                                const std::string&              programName,
                                const std::vector<std::string>  arguments)
{
    if (arguments.size() < 2) {
        errorStream << "Error: not enough arguments.\n\n";
        usage(errorStream, programName);
        return -1;
    }

    result->clear();
    for (auto argument: arguments) {
        std::uint32_t address;
        if (hauberk::InternetAddressUtil::fromDisplay(&address, argument)) {
            errorStream << "Invalid address: " << argument << "\n\n";
            usage(errorStream, programName);
            return -1;
        }
        result->push_back({argument, address});
    }

    return 0;
}

int ArgumentParser::toInterfaces(std::ostream&                   errorStream,
                                 InterfaceAddresses             *result,
                                 const std::string&              programName,
                                 const std::vector<std::string>  arguments)
{
    ArgumentAddresses addresses;
    if (toAddresses(errorStream, &addresses, programName, arguments)) {
        return -1;
    }

    using Interfaces = trammel::List<const struct pcap_if, trammel::Interface>;

    Interfaces  interfaces;
    std::string interfaceListError;
    if (trammel::InterfacesUtil::list(&interfaces, &interfaceListError)) {
        errorStream << interfaceListError << '\n';
        return -1;
    }

    result->clear();
    for (auto address: addresses) {
        std::string interfaceName;
        if (trammel::InterfacesUtil::fromAddress(&interfaceName,
                                                 address.second,
                                                 interfaces)) {
            errorStream << "Could not find " << address.first << " on any "
                           "interface.\n\n";
            usage(errorStream, programName);
            return -1;
        }
        result->push_back({interfaceName, address.second});
    }
    return 0;
}

std::ostream& ArgumentParser::usage(std::ostream&      stream,
                                    const std::string& programName)
{
    return stream << "Usage: " << programName <<
                     " <bind address> <gateway>...\n";
}

}  // close namespace 'conduit'

