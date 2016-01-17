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

#ifndef INCLUDED_VECTOR
#define INCLUDED_VECTOR
#include <vector>
#endif

namespace hauberk {

                             // =================
                             // struct BufferUtil
                             // =================

struct BufferUtil {
    // TYPES
    typedef std::vector<std::uint8_t>::iterator BytesIterator;

    // CLASS METHODS
    template <class DESTINATION>
    static void copy(DESTINATION *destination, const std::uint8_t *buffer);
        // TBD: contract

    template <class SOURCE>
    static void copy(std::uint8_t *buffer, const SOURCE& source);
        // TBD: contract

    static void reverseBits(std::uint8_t *value);
        // TBD: contract

    static void toHostOrder(std::uint16_t *value);
        // TBD: contract

    static void toHostOrder(std::uint32_t *value);
        // TBD: contract

    static void toNetworkOrder(std::uint16_t *value);
        // TBD: contract

    static void toNetworkOrder(std::uint32_t *value);
        // TBD: contract

    template <class TYPE>
    static TYPE slice(TYPE source, std::uint8_t start, std::uint8_t end);
        // TBD: contract
};

                             // -----------------
                             // struct BufferUtil
                             // -----------------

// CLASS METHODS
template <class DESTINATION>
void BufferUtil::copy(DESTINATION *destination, const std::uint8_t *buffer)
{
    auto bufferDestination = static_cast<std::uint8_t *>(
                             static_cast<void         *>(destination));
    std::copy(buffer, buffer + sizeof(DESTINATION), bufferDestination);
}

template <class SOURCE>
void BufferUtil::copy(std::uint8_t *buffer, const SOURCE& source)
{
    auto bufferSource = static_cast<const std::uint8_t *>(
                        static_cast<const void         *>(&source));
    std::copy(bufferSource, bufferSource + sizeof(SOURCE), buffer);
}

template <class TYPE>
TYPE BufferUtil::slice(TYPE source, std::uint8_t start, std::uint8_t end)
{
    return (source >> ((8 * sizeof(TYPE)) - end)) & ((1 << (end - start)) - 1);
}

}  // close namespace 'hauberk'

#endif
