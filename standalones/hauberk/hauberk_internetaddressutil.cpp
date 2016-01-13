// hauberk_internetaddressutil.cpp

#include <hauberk_internetaddressutil.h>

#include <arpa/inet.h>

namespace hauberk {

                         // -------------------------
                         // class InternetAddressUtil
                         // -------------------------

// CLASS METHODS
int InternetAddressUtil::displayToAddress(uint32_t           *address,
                                          const std::string&  display)
{
    in_addr internetAddress;
    if (inet_aton(display.c_str(), &internetAddress) == 0) {
        return -1;
    }

    *address = internetAddress.s_addr;
    return 0;
}

}  // close namespace 'hauberk'

