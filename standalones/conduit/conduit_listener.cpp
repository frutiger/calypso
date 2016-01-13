// conduit_listener.cpp

#include <conduit_listener.h>

#include <maxwell_queue.h>

#include <array>
#include <cassert>
#include <ostream>

namespace conduit {

                               // --------------
                               // class Listener
                               // --------------

// PRIVATE CLASS METHODS
int Listener::dispatchEvent(std::uintptr_t, void *userData)
{
    return static_cast<Listener *>(userData)->packetsReady();
}

// MODIFIERS
int Listener::packetsReady()
{
    assert(d_activated);

    trammel::CaptureMetadata *metadata;
    const uint8_t            *data;
    while (!d_capture.read(&metadata, &data)) {
        // TBD: process packet
    }
    return 0;
}

// CREATORS
Listener::Listener(const std::string& interface, std::uint32_t address)
: d_interface(interface)
, d_address(address)
, d_eventHandler(&Listener::dispatchEvent, this)
, d_readHandle()
, d_capture()
, d_activated()
, d_dataLinkType()
{
    // TBD: silence warning
    ++d_address;
}

// MANIPULATORS
int Listener::activate(std::ostream& errorStream, const maxwell::Queue& queue)
{
    assert(!d_activated);

    std::string openError;
    if (d_capture.open(d_interface, &openError)) {
        errorStream << "Failed to open listener capture: " << openError
                    << '\n';
        return -1;
    }

    if (d_capture.setNonblock(true)) {
        errorStream << "Failed to set nonblocking mode\n";
        return -1;
    }

    d_capture.setTimeout(10);
    if (d_capture.activate()) {
        errorStream << "Failed to activate listener\n";
        return -1;
    }
    d_activated = true;

    d_dataLinkType = d_capture.dataLinkType();
    uintptr_t listenDescriptor;
    if (d_capture.fileDescriptor(&listenDescriptor)) {
        errorStream << "Failed to get listener descriptor\n";
        return -1;
    }

    if (queue.setReadHandler(errorStream,
                             &d_readHandle,
                             listenDescriptor,
                             &d_eventHandler)) {
        return -1;
    }

    return 0;
}

}  // close namespace 'conduit'

