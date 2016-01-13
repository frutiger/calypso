// trammel_send.cpp

#include <trammel_send.h>

#include <hauberk_bufferutil.h>
#include <hauberk_internet.h>

#include <vector>

#include <pcap/pcap.h>

namespace trammel {

namespace {
static const std::uint32_t PROTOCOL_FAMILY_INTERNET = 2;
}  // close unnamed namespace

                                 // ----------
                                 // class Send
                                 // ----------

// CREATORS
Send::Send(const std::string&                interface,
           const hauberk::Ethernet::Address& hardwareAddress)
: d_interface(interface)
, d_hardwareAddress(hardwareAddress)
, d_handle(0, &pcap_close)
{
}

// MANIPULATORS
int Send::open(std::ostream& errorStream)
{
    char         errorBuffer[PCAP_ERRBUF_SIZE];
    struct pcap *capture = pcap_create(d_interface.c_str(), errorBuffer);
    if (!capture) {
        errorStream << "Failed to create pcap: " << errorBuffer;
        return -1;
    }
    else {
        d_handle.reset(capture);
    }

    int activationRc = pcap_activate(d_handle.get());
    if (activationRc) {
        switch (activationRc) {
            case PCAP_ERROR:
                errorStream << "unknown activation error";
            case PCAP_ERROR_ACTIVATED:
                errorStream << "already activated";
            case PCAP_ERROR_IFACE_NOT_UP:
                errorStream << "interface not up";
            case PCAP_ERROR_NO_SUCH_DEVICE:
                errorStream << "no such device";
            case PCAP_ERROR_PERM_DENIED:
                errorStream << "permission denied";
            case PCAP_ERROR_PROMISC_PERM_DENIED:
                errorStream << "promiscuous mode permission denied";
            case PCAP_ERROR_RFMON_NOTSUP:
                errorStream << "monitor mode not supported";
            case PCAP_WARNING:
                errorStream << "unknown activation warning";
            case PCAP_WARNING_PROMISC_NOTSUP:
                errorStream << "promiscuous mode not supported";
            case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
                errorStream << "timestamp format not supported";
            default:
                errorStream << "unknown activation error";
        }
        return -1;
    }

    d_datalinkType = pcap_datalink(d_handle.get());
    return 0;
}

int Send::write(const hauberk::Internet& internet)
{
}

}  // close namespace 'trammel'
