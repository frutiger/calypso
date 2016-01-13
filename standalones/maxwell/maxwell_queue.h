// maxwell_queue.h
#ifndef MAXWELL_QUEUE
#define MAXWELL_QUEUE

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

namespace maxwell {

class EventHandler;

                                // ===========
                                // class Queue
                                // ===========

class Queue {
  private:
    // DATA
    int           d_kernelQueue;
    std::uint64_t d_waitNanoseconds;

    // PRIVATE TYPES
                          // =======================
                          // class Queue::ReadHandle
                          // =======================

    class ReadHandle {
        // DATA
        int             d_kernelQueue;
        std::uintptr_t  d_identifer;

      public:
        // CREATORS
        ReadHandle();
            // TBD: contract

        ~ReadHandle();
            // TBD: contract

        // MANIPULATORS
        int enable(std::ostream&   errorStream,
                   int             kernelQueue,
                   std::uintptr_t  identifier,
                   EventHandler   *eventHandler);
    };

  public:
    // DELETED METHODS
    Queue(const Queue&)            = delete;
    Queue& operator=(const Queue&) = delete;

    // CREATORS
    explicit Queue(std::uint64_t waitNanoseconds);
        // TBD: contract

    // MANIPULATORS
    int create(std::ostream& errorStream);
        // TBD: contract

    // ACCESSORS
    int setReadHandler(std::ostream&          errorStream,
                       std::shared_ptr<void> *readHandle,
                       std::uintptr_t         identifier,
                       EventHandler          *handler) const;

    int start(std::ostream& errorStream) const;
        // TBD: contract
};

}  // close namespace 'maxwell'

#endif
