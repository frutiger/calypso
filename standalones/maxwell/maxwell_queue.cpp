// maxwell_queue.cpp

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
        std::abort();
    }
}

// MANIPULATORS
int Queue::ReadHandle::enable(std::ostream&             errorStream,
                              std::unique_ptr<Handler> *handler,
                              int                       kernelQueue,
                              std::uintptr_t            identifier)
{
    d_kernelQueue = kernelQueue;
    d_identifer   = identifier;
    d_handler     = std::move(*handler);

    struct kevent change = {
        identifier,   // identifier
        EVFILT_READ,  // filter
        EV_ADD,       // general flags,
        0,            // filter flags
        0,            // filter data
        &d_handler,   // user data
    };

    if (kevent(kernelQueue, &change, 1, 0, 0, 0) == -1) {
        errorStream << "Failed to add listener to kernel queue: "
                    << strerror(errno) << '\n';
        return -1;
    }

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
int Queue::setReadHandler(std::ostream&             errorStream,
                          Handle                   *readHandle,
                          std::unique_ptr<Handler> *handler,
                          std::uintptr_t            identifier) const
{
    std::shared_ptr<ReadHandle> handle(new ReadHandle);
    if (handle->enable(errorStream, handler, d_kernelQueue, identifier)) {
        return -1;
    }

    *readHandle = handle;
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
            auto *handler =
                      static_cast<std::unique_ptr<Handler> *>(events[i].udata);
            if ((**handler)(events[i].ident)) {
                errorStream << "User event handler returned error.\n";
                return -1;
            }
        }
    }
}

}  // close namespace 'maxwell'

