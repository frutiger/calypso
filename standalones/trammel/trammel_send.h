// trammel_send.h
#ifndef TRAMMEL_SEND
#define TRAMMEL_SEND

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

struct pcap;

namespace trammel {

                                 // ==========
                                 // class Send
                                 // ==========

class Send {
    // DATA
    std::unique_ptr<struct pcap, void (*)(struct pcap *)> d_handle;

  public:
    // DELETED METHODS
    Send(const Send&) = delete;
    Send& operator=(const Send&) = delete;

    // CREATORS
    explicit Send();
        // TBD: contract

    // MANIPULATORS
    int open(std::ostream& errorStream, const std::string& interface);
        // TBD: contract

    int write(const std::uint8_t *data, std::uint32_t length);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
