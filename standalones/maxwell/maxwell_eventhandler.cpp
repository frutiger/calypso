// maxwell_eventhandler.cpp

#include <maxwell_eventhandler.h>

namespace maxwell {

                             // ------------------
                             // class EventHandler
                             // ------------------

// CREATORS
EventHandler::EventHandler(Handler handler, void *userData)
: d_handler(handler)
, d_userData(userData)
{
}

// ACCESSORS
int EventHandler::handle(std::uintptr_t identifier) const
{
    return d_handler(identifier, d_userData);
}

}  // close namespace 'maxwell'

