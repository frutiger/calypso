// hauberk_internetaddressutil.cpp

#include <hauberk_internetaddressutil.h>

#include <arpa/inet.h>

namespace hauberk {

                         // -------------------------
                         // class InternetAddressUtil
                         // -------------------------

// CLASS METHODS
int InternetAddressUtil::fromDisplay(std::uint32_t      *address,
                                     const std::string&  display)
{
    struct in_addr internetAddress;
    if (inet_aton(display.c_str(), &internetAddress) == 0) {
        return -1;
    }

    *address = internetAddress.s_addr;
    return 0;
}

void InternetAddressUtil::toDisplay(std::string   *display,
                                    std::uint32_t  address)
{
    struct in_addr internetAddress = { address };
    display->assign(inet_ntoa(internetAddress));
}

}  // close namespace 'hauberk'

