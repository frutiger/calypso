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

    static void reverseBits(uint8_t *value);
        // TBD: contract

    static void toHostOrder(uint16_t *value);
        // TBD: contract

    static void toHostOrder(uint32_t *value);
        // TBD: contract

    template <class TYPE>
    static TYPE slice(TYPE source, std::uint8_t start, std::uint8_t end);
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
TYPE BufferUtil::slice(TYPE source, std::uint8_t start, std::uint8_t end)
{
    return (source >> ((8 * sizeof(TYPE)) - end)) & ((1 << (end - start)) - 1);
}

}  // close namespace 'hauberk'

#endif
