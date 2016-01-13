// maxwell_queue.cpp

#include <maxwell_eventhandler.h>
#include <maxwell_queue.h>

#include <array>

#include <sys/event.h>

namespace maxwell {

                          // -----------------------
                          // class Queue::ReadHandle
                          // -----------------------


// CREATORS
Queue::ReadHandle::ReadHandle()
: d_kernelQueue()
, d_identifer()
{
}

Queue::ReadHandle::~ReadHandle()
{
    if (!d_identifer) {
        return;
    }

    struct kevent change = {
        d_identifer,      // identifier
        EVFILT_READ,      // filter
        EV_DELETE,        // general flags,
        0,                // filter flags
        0,                // filter data
        0,                // user data
    };

    if (kevent(d_kernelQueue, &change, 1, 0, 0, 0) == -1) {
        abort();
    }
}

// MANIPULATORS
int Queue::ReadHandle::enable(std::ostream&   errorStream,
                              int             kernelQueue,
                              std::uintptr_t  identifier,
                              EventHandler   *eventHandler)
{
    struct kevent change = {
        identifier,    // identifier
        EVFILT_READ,   // filter
        EV_ADD,        // general flags,
        0,             // filter flags
        0,             // filter data
        eventHandler,  // user data
    };

    if (kevent(kernelQueue, &change, 1, 0, 0, 0) == -1) {
        errorStream << "Failed to add listener to kernel queue: "
                    << strerror(errno) << '\n';
        return -1;
    }

    d_kernelQueue = kernelQueue;
    d_identifer   = identifier;

    return 0;
}

                                // -----------
                                // class Queue
                                // -----------

// CREATORS
Queue::Queue(std::uint64_t waitNanoseconds)
: d_kernelQueue(-1)
, d_waitNanoseconds(waitNanoseconds)
{
}

// MANIPULATORS
int Queue::create(std::ostream& errorStream)
{
    d_kernelQueue = kqueue();
    if (d_kernelQueue == -1) {
        errorStream << "Failed to create kernel queue: " << strerror(errno)
                    << '\n';
        return -1;
    }
    return 0;
}

// ACCESSORS
int Queue::setReadHandler(std::ostream&          errorStream,
                          std::shared_ptr<void> *readHandle,
                          std::uintptr_t         identifier,
                          EventHandler          *handler) const
{
    std::shared_ptr<ReadHandle> handle(new ReadHandle);
    if (handle->enable(errorStream, d_kernelQueue, identifier, handler)) {
        return -1;
    }

    *readHandle = std::shared_ptr<void>(handle, 0);
    return 0;
}

int Queue::start(std::ostream& errorStream) const
{
    std::array<struct kevent, 0x100> events;
    time_t seconds       = d_waitNanoseconds / (1000 * 1000 * 1000);
    long   nanoseconds   = d_waitNanoseconds - (seconds * 1000 * 1000 * 1000);
    struct timespec wait = { seconds, nanoseconds };
    while (true) {
        int numEvents = kevent(d_kernelQueue,
                               0,
                               0,
                               events.begin(),
                               events.size(),
                               &wait);
        if (numEvents == -1) {
            errorStream << "Failed to get kernel queue events.\n";
            return -1;
        }

        for (int i = 0; i < numEvents; ++i) {
            EventHandler *eventHandler = static_cast<EventHandler *>(
                                                              events[i].udata);
            if (eventHandler->handle(events[i].ident)) {
                errorStream << "User event handler returned error.\n";
                return -1;
            }
        }
    }
}

}  // close namespace 'maxwell'
