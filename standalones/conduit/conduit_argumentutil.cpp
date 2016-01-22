// conduit_argumentutil.cpp

#include <conduit_argumentutil.h>

#include <hauberk_internetaddressutil.h>
#include <trammel_address.h>
#include <trammel_interface.h>
#include <trammel_interfacesutil.h>
#include <trammel_list.h>

namespace conduit {

                            // ------------------
                            // class ArgumentUtil
                            // ------------------

// CLASS METHODS
int ArgumentUtil::toSimplex(std::ostream&       errorStream,
                            Simplex            *result,
                            const Interfaces&   interfaces,
                            const std::string&  argument)
{
    if (hauberk::InternetAddressUtil::fromDisplay(&result->d_address,
                                                  argument)) {
        errorStream << "Invalid address: " << argument << "\n\n";
        return -1;                                                    // RETURN
    }

    if (trammel::InterfacesUtil::fromAddress(&result->d_interfaceName,
                                             result->d_address,
                                             interfaces)) {
        errorStream << "Could not find " << result->d_address << " on any "
                       "interface.\n\n";
        return -1;                                                // RETURN
    }

    return 0;
}

int ArgumentUtil::toDuplexTunnel(std::ostream&       errorStream,
                                 Duplex::Tunnel     *result,
                                 const std::string&  argument)
{
    std::string::size_type delimiter = argument.find(':');
    if (delimiter == std::string::npos) {
        errorStream << "Missing ':' in: " << argument << "\n\n";
        return -1;                                                    // RETURN
    }

    std::string address(argument.begin(), argument.begin() + delimiter);
    if (hauberk::InternetAddressUtil::fromDisplay(&result->d_address,
                                                  address)) {
        errorStream << "Invalid address: " << address << "\n\n";
        return -1;                                                    // RETURN
    }

    std::string gateway(argument.begin() + delimiter + 1, argument.end());
    if (hauberk::InternetAddressUtil::fromDisplay(&result->d_gateway,
                                                  gateway)) {
        errorStream << "Invalid address: " << gateway << "\n\n";
        return -1;                                                    // RETURN
    }

    return 0;
}

int ArgumentUtil::toDuplex(std::ostream&       errorStream,
                           Duplex             *result,
                           const Interfaces&   interfaces,
                           const std::string&  argument)
{
    if (toDuplexTunnel(errorStream, &result->d_tunnel, argument)) {
        return -1;                                                // RETURN
    }

    if (trammel::InterfacesUtil::fromAddress(&result->d_interfaceName,
                                             result->d_tunnel.d_address,
                                             interfaces)) {
        errorStream << "Could not find " << result->d_tunnel.d_address
                    << " on any interface.\n\n";
        return -1;                                                // RETURN
    }

    return 0;
}

}  // close namespace 'conduit'

