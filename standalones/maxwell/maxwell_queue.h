// maxwell_queue.h
#ifndef MAXWELL_QUEUE
#define MAXWELL_QUEUE

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

#ifndef INCLUDED_FUNCTIONAL
#define INCLUDED_FUNCTIONAL
#include <functional>
#endif

#ifndef INCLUDED_OSTREAM
#define INCLUDED_OSTREAM
#include <ostream>
#endif

namespace maxwell {

                                // ===========
                                // class Queue
                                // ===========

class Queue {
  public:
    // TYPES
    typedef std::function<int (std::uintptr_t)> Handler;

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
        int                      d_kernelQueue;
        std::uintptr_t           d_identifer;
        std::unique_ptr<Handler> d_handler;

      public:
        // CREATORS
        ReadHandle();
            // TBD: contract

        ~ReadHandle();
            // TBD: contract

        // MANIPULATORS
        int enable(std::ostream&             errorStream,
                   std::unique_ptr<Handler> *handler,
                   int                       kernelQueue,
                   std::uintptr_t            identifier);
    };

  public:
    // TYPES
    typedef std::shared_ptr<void> Handle;

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
    int start(std::ostream& errorStream) const;
        // TBD: contract

    int setReadHandler(std::ostream&             errorStream,
                       Handle                   *handle,
                       std::unique_ptr<Handler> *handler,
                       std::uintptr_t            identifier) const;
        // TBD: contract
};

}  // close namespace 'maxwell'

#endif
