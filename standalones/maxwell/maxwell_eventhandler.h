// maxwell_eventhandler.h

#ifndef MAXWELL_EVENTHANDLER
#define MAXWELL_EVENTHANDLER

#ifndef INCLUDED_CSTDINT
#define INCLUDED_CSTDINT
#include <cstdint>
#endif

namespace maxwell {

                             // ==================
                             // class EventHandler
                             // ==================

class EventHandler {
  public:
    // TYPES
    typedef int (*Handler)(std::uintptr_t identifier, void *userData);

  private:
    // DATA
    Handler  d_handler;
    void    *d_userData;

  public:
    // CREATORS
    EventHandler(Handler handler, void *userData);
        // TBD: contract

    // ACCESSORS
    int handle(std::uintptr_t identifier) const;
        // TBD: contract
};

}  // close namespace 'maxwell'

#endif
