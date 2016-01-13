// trammel_capture.cpp

#include <trammel_capture.h>

#include <pcap/pcap.h>

#include <cassert>

namespace trammel {

                           // ---------------------
                           // class CaptureMetadata
                           // ---------------------

// CREATORS
CaptureMetadata::CaptureMetadata()
: d_header()
{
}

CaptureMetadata::CaptureMetadata(const struct pcap_pkthdr *header)
: d_header(header)
{
}

// ACCESSORS
std::chrono::time_point<std::chrono::system_clock>
CaptureMetadata::time() const
{
    return std::chrono::time_point<std::chrono::system_clock>(
                              std::chrono::seconds(d_header->ts.tv_sec) +
                              std::chrono::microseconds(d_header->ts.tv_usec));
}

std::uint32_t CaptureMetadata::capturedDataLength() const
{
    return d_header->caplen;
}

std::uint32_t CaptureMetadata::dataLength() const
{
    return d_header->len;
}

                               // -------------
                               // class Capture
                               // -------------
// PRIVATE CLASS METHODS
Capture::ActivationError Capture::toActivationError(int error)
{
    switch (error) {
        case PCAP_ERROR:
            return ActivationError::Error;
        case PCAP_ERROR_ACTIVATED:
            return ActivationError::ErrorAlreadyActivated;
        case PCAP_ERROR_IFACE_NOT_UP:
            return ActivationError::ErrorInterfaceNotUp;
        case PCAP_ERROR_NO_SUCH_DEVICE:
            return ActivationError::ErrorNoSuchDevice;
        case PCAP_ERROR_PERM_DENIED:
            return ActivationError::ErrorPermissionDenied;
        case PCAP_ERROR_PROMISC_PERM_DENIED:
            return ActivationError::ErrorPromiscuousPermissionDenied;
        case PCAP_ERROR_RFMON_NOTSUP:
            return ActivationError::ErrorMonitoringNotSupported;
        case PCAP_WARNING:
            return ActivationError::Warning;
        case PCAP_WARNING_PROMISC_NOTSUP:
            return ActivationError::WarningPromiscuousNotSupported;
        case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
            return ActivationError::WarningTimestampNotSupported;
    }
    return ActivationError::Unknown;
}

// CREATORS
Capture::Capture()
: d_handle(0, &pcap_close)
, d_activated()
, d_metadata()
{
}

Capture::~Capture()
{
}

// MANIPULATORS
int Capture::activate(ActivationError *error)
{
    int rc = pcap_activate(d_handle.get());
    if (rc) {
        if (error) {
            *error = toActivationError(rc);
        }
        return -1;
    }
    d_activated = true;
    return 0;
}

int Capture::dataLinkType()
{
    assert(d_activated);
    return pcap_datalink(d_handle.get());
}

int Capture::fileDescriptor(std::uintptr_t *fileDescriptor)
{
    int descriptor = pcap_get_selectable_fd(d_handle.get());
    if (descriptor == -1) {
        return -1;
    }
    *fileDescriptor = static_cast<uintptr_t>(descriptor);
    return 0;
}

int Capture::open(const std::string& interface, std::string *error)
{
    char         errorBuffer[PCAP_ERRBUF_SIZE];
    struct pcap *capture = pcap_create(interface.c_str(), errorBuffer);
    if (!capture) {
        if (error) {
            error->assign(errorBuffer);
        }
        return -1;
    }
    else {
        d_handle.reset(capture);
    }

    return 0;
}

int Capture::setNonblock(int nonblock, std::string *error)
{
    assert(d_handle);

    char errorBuffer[PCAP_ERRBUF_SIZE];
    if (pcap_setnonblock(d_handle.get(), nonblock, errorBuffer) == -1) {
        if (error) {
            error->assign(errorBuffer);
        }
        return -1;
    }
    return 0;
}

int Capture::read(CaptureMetadata **metadata, const uint8_t **capturedData)
{
    struct pcap_pkthdr *header;
    if (pcap_next_ex(d_handle.get(), &header, capturedData) != 1) {
        return -1;
    }
    d_metadata = CaptureMetadata(header);
    *metadata  = &d_metadata;
    return 0;
}

void Capture::setSnapshotLength(int snapshotLength)
{
    assert(d_handle);
    assert(!d_activated);
    int rc = pcap_set_snaplen(d_handle.get(), snapshotLength);
    assert(!rc);
}

void Capture::setTimeout(int timeoutMilliseconds)
{
    assert(d_handle);
    assert(!d_activated);
    int rc = pcap_set_timeout(d_handle.get(), timeoutMilliseconds);
    assert(!rc);
}

}  // close namespace 'trammel'

