// trammel_capture.h
#ifndef TRAMMEL_CAPTURE
#define TRAMMEL_CAPTURE

#ifndef INCLUDED_CHRONO
#define INCLUDED_CHRONO
#include <chrono>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING
#include <string>
#endif

struct pcap;
struct pcap_pkthdr;

namespace trammel {

                           // =====================
                           // class CaptureMetadata
                           // =====================

class CaptureMetadata {
    // DATA
    const struct pcap_pkthdr *d_header;

  public:
    // CREATORS
    explicit CaptureMetadata();
        // TBD: contract

    explicit CaptureMetadata(const struct pcap_pkthdr *header);
        // TBD: contract

    // ACCESSORS
    std::chrono::time_point<std::chrono::system_clock> time() const;
        // TBD: contract

    std::uint32_t capturedDataLength() const;
        // TBD: contract

    std::uint32_t dataLength() const;
        // TBD: contract
};

                               // =============
                               // class Capture
                               // =============

class Capture {
  public:
    // TYPES
    enum class ActivationError {
        Unknown,
        Error,
        ErrorAlreadyActivated,
        ErrorInterfaceNotUp,
        ErrorNoSuchDevice,
        ErrorPermissionDenied,
        ErrorPromiscuousPermissionDenied,
        ErrorMonitoringNotSupported,
        Warning,
        WarningPromiscuousNotSupported,
        WarningTimestampNotSupported,
    };

    enum class LinkType {
        Unknown,
        Loopback,
        Ethernet,
    };

  private:
    // PRIVATE CLASS METHODS
    static ActivationError toActivationError(int error);
        // TBD: contract

    // DATA
    std::unique_ptr<struct pcap, void (*)(struct pcap *)> d_handle;
    bool                                                  d_activated;
    CaptureMetadata                                       d_metadata;

  public:
    // CREATORS
    explicit Capture();
        // TBD: contract

    ~Capture();
        // TBD: contract

    // MANIPULATORS
    int activate(ActivationError *error = 0);
        // TBD: contract

    LinkType linkType();
        // TBD: contract

    int fileDescriptor(std::uintptr_t *fileDescriptor);
        // TBD: contract

    int open(const std::string& interface, std::string *error = 0);
        // TBD: contract

    int read(const CaptureMetadata **metadata, const uint8_t **capturedData);
        // TBD: contract

    int setNonblock(int nonblock, std::string *error = 0);
        // TBD: contract

    void setSnapshotLength(int snapshotLength);
        // TBD: contract

    void setTimeout(int timeoutMilliseconds);
        // TBD: contract
};

}  // close namespace 'trammel'

#endif
