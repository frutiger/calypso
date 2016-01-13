// trammel_iterator.h
#ifndef TRAMMEL_ITERATOR
#define TRAMMEL_ITERATOR

#include <memory>

namespace trammel {

                               // ==============
                               // class Iterator
                               // ==============

template <class PCAP_TYPE, class TRAMMEL_TYPE>
class Iterator {
    // FRIENDS
    template <class P, class Z>
    friend bool operator==(const Iterator<P, Z>& lhs,
                           const Iterator<P, Z>& rhs);

    // DATA
    std::shared_ptr<const PCAP_TYPE> d_current;
    TRAMMEL_TYPE                      d_value;

  public:
    // CREATORS
    explicit Iterator();
        // TBD: contract

    explicit Iterator(const std::shared_ptr<const PCAP_TYPE>& start);
        // TBD: contract

    // MANIPULATORS
    void operator++();
        // TBD: contract

    // ACCESSORS
    const TRAMMEL_TYPE& operator *() const;
        // TBD: contract

    const TRAMMEL_TYPE *operator->() const;
        // TBD: contract
};

// OPERATORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
bool operator==(const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& lhs,
                const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& rhs);
    // TBD: contract

template <class PCAP_TYPE, class TRAMMEL_TYPE>
bool operator!=(const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& lhs,
                const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& rhs);
    // TBD: contract

                         // --------------
                         // class Iterator
                         // --------------

// CREATORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
Iterator<PCAP_TYPE, TRAMMEL_TYPE>::Iterator()
: d_current()
, d_value(d_current)
{
}

template <class PCAP_TYPE, class TRAMMEL_TYPE>
Iterator<PCAP_TYPE, TRAMMEL_TYPE>::Iterator(
                                 const std::shared_ptr<const PCAP_TYPE>& start)
: d_current(start)
, d_value(d_current)
{
}

// MANIPULATORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
void Iterator<PCAP_TYPE, TRAMMEL_TYPE>::operator++()
{
    d_current = std::shared_ptr<const PCAP_TYPE>(d_current, d_current->next);
    d_value = TRAMMEL_TYPE(d_current);
}

// ACCESSORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
const TRAMMEL_TYPE& Iterator<PCAP_TYPE, TRAMMEL_TYPE>::operator *() const
{
    return d_value;
}

template <class PCAP_TYPE, class TRAMMEL_TYPE>
const TRAMMEL_TYPE *Iterator<PCAP_TYPE, TRAMMEL_TYPE>::operator->() const
{
    return &d_value;
}

// OPERATORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
bool operator==(const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& lhs,
                const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& rhs)
{
    return !lhs.d_current && !rhs.d_current;
}

template <class PCAP_TYPE, class TRAMMEL_TYPE>
bool operator!=(const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& lhs,
                const Iterator<PCAP_TYPE, TRAMMEL_TYPE>& rhs)
{
    return !(lhs == rhs);
}

} // close namespace 'trammel'

#endif
