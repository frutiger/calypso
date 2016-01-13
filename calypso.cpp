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

static int         g_queue;
static DnsRequests g_dnsRequests;

// TYPES

struct Internet {
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

static int parseEndpointId(EndpointId *id, const std::string& argument)
{
    auto atPosition = argument.find('@');
    if (atPosition == std::string::npos) {
        return -1;
    }

    id->interface = argument.substr(atPosition + 1);

    auto addressString = argument.substr(0, atPosition);
    struct in_addr address;
    if (0 == inet_aton(addressString.c_str(), &address)) {
        return -1;
    }
    id->address = address.s_addr;

    return 0;
}

static int parseArguments(std::vector<EndpointId>         *ids,
                          const std::vector<std::string>&  arguments)
{
    if (arguments.size() < 2) {
        return -1;
    }

    ids->clear();
    for (auto argument: arguments) {
        EndpointId id;
        if (parseEndpointId(&id, argument)) {
            return -1;
        }
        ids->push_back(id);
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

static uint16_t networkToHost(uint16_t source)
{
    return ntohs(source);
}

/*
static uint32_t networkToHost(uint32_t source)
{
    return ntohl(source);
}
*/

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

static int onSourcePacket(unsigned char *data,
                          uint32_t       dataLength,
                          void          *userData)
{
    Internet internet;
    copy(&internet, data);

    if (internet.family != AF_INET || internet.version != INET) {
        // TBD: use a 'BPF' program for this
        return 0;
    }

    std::vector<Endpoint> endpoints =
                               *static_cast<std::vector<Endpoint> *>(userData);

    if (endpoints[0].address != internet.sourceAddress) {
        // TBD: use a 'BPF' program for this
        return 0;
    }

    if (internet.protocol == IPPROTO_UDP) {
        int internetHeaderSize = 4 * (internet.headerWords);
        Udp udp;
        copy(&udp, data + 4 + internetHeaderSize);

        if (networkToHost(udp.destinationPort) == DNS_PORT) {
            if (g_dnsRequests.size() == (1 << 16)) {
                std::cerr << "DNS request table full\n";
                return -1;
            }

            Dns dns;
            copy(&dns, data + 4 + internetHeaderSize + sizeof(Udp));
            uint16_t transactionId;
            while (true) {
                arc4random_buf(&transactionId, sizeof(transactionId));
                if (g_dnsRequests.insert(
                    std::make_pair(transactionId, dns.transactionId)).second) {
                    break;
                }
            }

            std::vector<unsigned char> dnsRequest;
            dnsRequest.reserve(dataLength);
            std::copy_n(data, dataLength, std::back_inserter(dnsRequest));
            Dns newDns;
            newDns.transactionId = transactionId;
            //copy(dnsRequest.data() + 4 + internetHeaderSize + sizeof(Udp),
                 //newDns);

            write(endpoints[1].packetFilter,
                  dnsRequest.data(),
                  dnsRequest.size());
        }
    }
    //std::cout.write(reinterpret_cast<char *>(data + 4), length - 4);
    /*
    if (*(data + 0x14) != 127 || *(data + 0x15) != 0 ||
        *(data + 0x16) !=   0 || *(data + 0x17) != 2) {
        return;
    }
    std::copy_n(reinterpret_cast<unsigned char *>(&info.address),
                4,
                data + 0x14);
    write(info.packetFilter, data + 4, length - 4); // ip header?
    */
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
    srandomdev();

    std::vector<std::string> arguments(argv + 1, argv + argc);
    std::vector<EndpointId>  ids;
    if (parseArguments(&ids, arguments)) {
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

    g_queue = kqueue();
    if (g_queue == -1) {
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
    if (kevent(g_queue, &initialSet, 1, 0, 0, 0)) {
        std::cerr << "Failed to add source endpoint to kernel queue\n";
        return -1;
    }

    struct kevent events[0x100];
    while (true) {
        struct timespec fiveSeconds = { 5 };
        auto numEvents = kevent(g_queue,
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

