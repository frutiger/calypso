// trammel_list.h
#ifndef TRAMMEL_LIST
#define TRAMMEL_LIST

#include <trammel_iterator.h>

#include <memory>

namespace trammel {

                                 // ==========
                                 // class List
                                 // ==========

template <class PCAP_TYPE, class TRAMMEL_TYPE>
class List {
    // DATA
    std::shared_ptr<const PCAP_TYPE> d_head;

  public:
    // TYPES
    using iterator = Iterator<PCAP_TYPE, TRAMMEL_TYPE>;

    // CREATORS
    explicit List();
        // TBD: contract

    explicit List(const std::shared_ptr<const PCAP_TYPE>& head);
        // TBD: contract

    // ACCESSORS
    iterator begin() const;
        // TBD: contract
};

                                 // ----------
                                 // class List
                                 // ----------

// CREATORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
List<PCAP_TYPE, TRAMMEL_TYPE>::List()
: d_head()
{
}

template <class PCAP_TYPE, class TRAMMEL_TYPE>
List<PCAP_TYPE, TRAMMEL_TYPE>::List(
                                  const std::shared_ptr<const PCAP_TYPE>& head)
: d_head(head)
{
}

// ACCESSORS
template <class PCAP_TYPE, class TRAMMEL_TYPE>
typename List<PCAP_TYPE, TRAMMEL_TYPE>::iterator
List<PCAP_TYPE, TRAMMEL_TYPE>::begin() const
{
    return iterator(d_head);
}

}  // close namespace 'trammel'

#endif
