//calypso.cpp

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

#include <assert.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <net/bpf.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>

namespace {

// CONSTANTS

static const uint8_t  INET     =  4;
static const uint16_t DNS_PORT = 53;

// GLOBAL DATA

typedef std::unordered_map<uint16_t, uint16_t> DnsRequests;

static DnsRequests g_dnsRequests;

// TYPES

struct Ip {
    uint32_t      family;
    uint8_t       headerWords         :  4;
    uint8_t       version             :  4;
    uint8_t       dscpEcn;
    uint16_t      length;
    uint16_t      identification;
    uint16_t      flagsFragmentOffset;
    uint8_t       timeToLive;
    uint8_t       protocol;
    uint16_t      headerChecksum;
    uint32_t      sourceAddress;
    uint32_t      destinationAddress;
    unsigned char options[];
};

struct Udp {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
};

struct Dns {
    uint16_t transactionId;
};

struct PacketFilterEvent {
    int           (*handler)(uintptr_t     packetFilter,
                             unsigned int  packetSize,
                             void         *userData);
    unsigned int    packetSize;
    void           *userData;
};

struct Interface {
    uint32_t               address;
    bool                   isLoopback;
    std::array<uint8_t, 8> hardwareAddress
};

struct EndpointId {
    std::string interface;
    uint32_t    address;
};

struct Endpoint {
    int      packetFilter;
    uint32_t address;
};

// ARGUMENT PARSING

static std::ostream& usage(std::ostream& stream, const std::string& name)
{
    return stream << "Usage: " << name << " <source endpoint> "
                     "<destination endpoint>...\n"
                     "        where <endpoint> = <address>@<interface>\n";
}

static int parseArguments(std::vector<uint32_t>           *addresses,
                          const std::vector<std::string>&  arguments)
{
    addresses->clear();
    if (arguments.size() < 2) {
        return -1;
    }

    for (auto argument: arguments) {
        struct in_addr address;
        if (0 == inet_aton(addressString.c_str(), &address)) {
            return -1;
        }
        addresses.push_back(address.s_addr);
    }
    return 0;
}

// NETWORK BYTE MANIPULATION

template <class T>
void copy(T *destination, const unsigned char *source)
{
    std::copy_n(source,
                sizeof(T),
                static_cast<unsigned char *>(
                                            static_cast<void *>(destination)));
}

template <class T>
void copy(unsigned char *destination, const T& source)
{
    std::copy_n(static_cast<const unsigned char *>(
                                          static_cast<const void *>(&source)),
                sizeof(T),
                destination);
}

template <class DESTINATION, class SOURCE>
void copy(DESTINATION *destination, const SOURCE& source)
{
    std::copy_n(static_cast<const unsigned char *>(
                                          static_cast<const void *>(&source)),
                sizeof(SOURCE),
                static_cast<unsigned char *>(
                                            static_cast<void *>(destination)));
}

static uint16_t networkToHost(uint16_t source)
{
    return ntohs(source);
}

static uint16_t hostToNetwork(uint16_t source)
{
    return htonl(source);
}
/*
static uint32_t networkToHost(uint32_t source)
{
    return ntohl(source);
}
*/

// INTERFACE UTILITIES

static int getInterface(Interface *interface, uint32_t address)
{
    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor == -1) {
        std::cerr << "Failed to open socket while determining interface\n";
        return -1;
    }

    connect(socketDescriptor
}

// BERKELEY PACKET FILTER UTILITIES

static int openPacketFilter(int *packetFilter)
{
    std::stringstream deviceName;

    for (int i = 0; i < 256; ++i) {
        deviceName << "/dev/bpf" << i;
        *packetFilter = open(deviceName.str().c_str(), O_RDWR);
        if (-1 != *packetFilter) {
            if (i == 255) {
                return -1;
            }
            return 0;
        }
        errno = 0;
        deviceName.str("");
    }
    return -1;
}

static int initializePacketFilter(int                 packetFilter,
                                  const std::string&  interface)
{
    struct ifreq interfaceRequest = {};
    std::copy(interface.begin(), interface.end(), interfaceRequest.ifr_name);
    if (-1 == ioctl(packetFilter, BIOCSETIF, &interfaceRequest)) {
        std::cerr << "Failed to set interface on packet filter\n";
        return -1;
    }

    auto immediate = 1;
    if (-1 == ioctl(packetFilter, BIOCIMMEDIATE, &immediate)) {
        std::cerr << "Failed to set immediate mode\n";
        return -1;
    }

    return 0;
}

static int getPacketSize(unsigned int *packetSize, int packetFilter)
{
    if (-1 == ioctl(packetFilter, BIOCGBLEN, packetSize)) {
        std::cerr << "Failed to get packet buffer size\n";
        return -1;
    }

    return 0;
}

static int openEndpoint(Endpoint *endpoint, const EndpointId& id)
{
    int packetFilter;
    if (openPacketFilter(&packetFilter)) {
        std::cerr << "Failed to open packet filter\n";
        return -1;
    }

    if (initializePacketFilter(packetFilter, id.interface)) {
        return -1;
    }

    endpoint->packetFilter = packetFilter;
    endpoint->address      = id.address;
    return 0;
}

// SOURCE RECEIVER HANDLERS

static int onSourcePacket(const unsigned char *data,
                          uint32_t             dataLength,
                          void                *userData)
{
    Ip inPacket;
    copy(&inPacket, data);

    std::vector<Endpoint> endpoints =
                               *static_cast<std::vector<Endpoint> *>(userData);

    if (endpoints[0].address != inPacket.sourceAddress) {
        // TBD: use a 'BPF' program for this
        return 0;
    }

    if (inPacket.protocol == IPPROTO_UDP) {
        int internetHeaderSize = 4 * (inPacket.headerWords);
        Udp inUdp;
        copy(&inUdp, data + 4 + internetHeaderSize);

        if (networkToHost(inUdp.destinationPort) == DNS_PORT) {
            if (g_dnsRequests.size() == (1 << 16)) {
                std::cerr << "DNS request table full\n";
                return -1;
            }

            Dns inDns;
            copy(&inDns, data + 4 + internetHeaderSize + sizeof(Udp));
            uint16_t outTxId;
            while (true) {
                arc4random_buf(&outTxId, sizeof(outTxId));
                auto dnsRequest = std::make_pair(outTxId, inDns.transactionId);
                if (g_dnsRequests.insert(dnsRequest).second) {
                    break;
                }
            }

            std::vector<unsigned char> outPacket(data, dataLength);
            copy(dnsRequest.data() + 4

            Dns outDns;
            outDns.transactionId = transactionId;
            //copy(dnsRequest.data() + 4 + internetHeaderSize + sizeof(Udp),
                 //newDns);

            int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            struct sockaddr_in dest = {
                AF_INET,
                hostToNetwork(53),
                { endpoints[1].address }
            };
            struct sockaddr d;
            copy(&d, dest);
            sendto(s,
                   dnsRequest.data() + 4,
                   dnsRequest.size() - 4,
                   0,
                   &d,
                   sizeof(struct sockaddr_in));
        }
    }
    return 0;
}

static int onSourceEndpointData(uintptr_t     packetFilter,
                                unsigned int  packetSize,
                                void         *userData) {
    std::vector<unsigned char> packet(packetSize);

    while (true) {
        int bytesRead = read(packetFilter, packet.data(), packet.size());
        if (-1 == bytesRead) {
            std::cerr << "Failed to read: " << strerror(errno) << '\n';
            return -1;
        }

        unsigned char *packetData = packet.data();
        while (0 < bytesRead) {
            struct bpf_hdr header;
            copy(&header, packetData);

            unsigned char *data       = packetData + header.bh_hdrlen;
            uint32_t       dataLength = header.bh_caplen;

            packetData += BPF_WORDALIGN(header.bh_hdrlen + header.bh_caplen);
            bytesRead  -= BPF_WORDALIGN(header.bh_hdrlen + header.bh_caplen);

            onSourcePacket(data, dataLength, userData);
        }

        packet.clear();
        packet.resize(packetSize);
    }
}

}  // close unnamed namespace

int main(int argc, char **argv)
{
    std::vector<std::string> arguments(argv + 1, argv + argc);
    std::vector<uint32_t>    addresses;
    if (parseArguments(&addresses, arguments)) {
        usage(std::cerr, argv[0]);
        return -1;
    }

    std::vector<Endpoint> endpoints;
    for (auto id: ids) {
        Endpoint endpoint;
        if (openEndpoint(&endpoint, id)) {
            return -1;
        }
        endpoints.push_back(endpoint);
    }

    int queue = kqueue();
    if (queue == -1) {
        std::cerr << "Failed to open kernel queue\n";
        return -1;
    }

    unsigned int packetSize;
    if (getPacketSize(&packetSize, endpoints[0].packetFilter)) {
        return -1;
    }

    PacketFilterEvent event {
        &onSourceEndpointData,
        packetSize,
        &endpoints
    };
    struct kevent initialSet {
        static_cast<unsigned int>(endpoints[0].packetFilter),
        EVFILT_READ,
        EV_ADD,
        0,
        0,
        &event
    };
    if (kevent(queue, &initialSet, 1, 0, 0, 0)) {
        std::cerr << "Failed to add source endpoint to kernel queue\n";
        return -1;
    }

    struct kevent events[0x100];
    while (true) {
        struct timespec fiveSeconds = { 5 };
        auto numEvents = kevent(queue,
                                0,
                                0,
                                events,
                                sizeof(events) / sizeof(events[0]),
                                &fiveSeconds);
        if (numEvents == -1) {
            std::cerr << "Failed to retrieve queue events: " << strerror(errno)
                      << '\n';
            return -1;
        }

        for (auto i = 0; i < numEvents; ++i) {
            auto handler = static_cast<PacketFilterEvent *>(events[i].udata);
            if (handler->handler(events[i].ident,
                                 handler->packetSize,
                                 handler->userData)) {
                return -1;
            }
        }
    }
}

