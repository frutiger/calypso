// hauberk_internetaddressutil.h
#ifndef HAUBERK_INTERNETADDRESSUTIL
#define HAUBERK_INTERNETADDRESSUTIL

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

namespace hauberk {

                         // =========================
                         // class InternetAddressUtil
                         // =========================

struct InternetAddressUtil {
    // CLASS METHODS
    static int displayToAddress(std::uint32_t      *address,
                                const std::string&  display);
};

}  // close namespace 'hauberk'

#endif
