// hauberk_bufferutil.h
#ifndef HAUBERK_BUFFERUTIL
#define HAUBERK_BUFFERUTIL

#ifndef INCLUDED_ALGORITHM
#define INCLUDED_ALGORITHM
#include <algorithm>
#endif

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace hauberk {

                             // =================
                             // struct BufferUtil
                             // =================

struct BufferUtil {
    // CLASS METHODS
    template <class DESTINATION, class SOURCE>
    static void copy(DESTINATION *destination, const SOURCE& source);
        // TBD: contract

    static uint8_t reverseBits(uint8_t source);
        // TBD: contract

    template <class TYPE>
    static TYPE slice(const TYPE& source, uint8_t start, uint8_t end);
};

                             // -----------------
                             // struct BufferUtil
                             // -----------------

// CLASS METHODS
template <class DESTINATION, class SOURCE>
void BufferUtil::copy(DESTINATION *destination, const SOURCE& source)
{
    std::copy_n(static_cast<const std::uint8_t *>(
                                           static_cast<const void *>(&source)),
                sizeof(DESTINATION),
                static_cast<std::uint8_t *>(static_cast<void *>(destination)));
}

template <class TYPE>
TYPE BufferUtil::slice(const TYPE& source, uint8_t start, uint8_t end)
{
    TYPE result = 0;
    TYPE mask   = 1;
    for (uint8_t i = 0; i < sizeof(TYPE) * 8; ++i) {
        if (i >= start && i < end && (source & mask)) {
            result |= 1;
        }
        result <<= 1;
        mask   <<= 1;
    }
    return result;
}

}  // close namespace 'hauberk'

#endif
