// trammel_send.cpp

#include <trammel_send.h>

#include <pcap/pcap.h>

namespace trammel {

                                 // ----------
                                 // class Send
                                 // ----------

// CREATORS
Send::Send()
: d_handle(0, &pcap_close)
{
}

// MANIPULATORS
int Send::open(std::ostream& errorStream, const std::string& interface)
{
    char         errorBuffer[PCAP_ERRBUF_SIZE];
    struct pcap *capture = pcap_create(interface.c_str(), errorBuffer);
    if (!capture) {
        errorStream << "Failed to create pcap: " << errorBuffer;
        return -1;
    }
    else {
        d_handle.reset(capture);
    }

    return 0;
}

int Send::write(const std::uint8_t *data, std::uint32_t length)
{
    if (pcap_sendpacket(d_handle.get(), data, length)) {
        return -1;
    }
    return 0;
}

}  // close namespace 'trammel'
