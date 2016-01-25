// calypso.m.cpp

#include <conduit_argumentutil.h>
#include <conduit_listener.h>
#include <maxwell_queue.h>
#include <trammel_interfacesutil.h>

#include <iostream>
#include <vector>

namespace {

static const std::uint64_t TEN_MS_AS_NS = 10 * 1000 * 1000;

static std::ostream& usage(std::ostream&      stream,
                           const std::string& programName)
{
    return stream << "Usage: " << programName <<
                     " <src> <gwy src>:<gwy dst>...\n";
}

}  // close unnamed namespace

int main(int argc, char **argv)
{
    std::vector<std::string> arguments(argv, argv + argc);

    if (arguments.size() < 2) {
        usage(std::cerr, arguments[0]);
        return -1;                                                    // RETURN
    }

    conduit::ArgumentUtil::Interfaces interfaces;
    if (trammel::InterfacesUtil::list(std::cerr, &interfaces)) {
        return -1;                                                    // RETURN
    }

    // TBD: make loopback address instead of requiring it
    conduit::ArgumentUtil::Simplex input;
    if (conduit::ArgumentUtil::toSimplex(std::cerr,
                                         &input,
                                         interfaces,
                                         arguments[1])) {
        usage(std::cerr, arguments[0]);
        return -1;                                                    // RETURN
    }

    conduit::ArgumentUtil::Duplexes outputs;
    for (auto argument  = arguments.begin() + 2;
              argument != arguments.end();
            ++argument) {
        outputs.resize(outputs.size() + 1);
        if (conduit::ArgumentUtil::toDuplex(std::cerr,
                                            &outputs.back(),
                                            interfaces,
                                            *argument)) {
            usage(std::cerr, arguments[0]);
            return -1;
        }
    }

    maxwell::Queue queue(TEN_MS_AS_NS);
    if (queue.create(std::cerr)) {
        return -1;
    }

    conduit::Listener listener(input, outputs.begin(), outputs.end());
    if (listener.open(std::cerr, queue)) {
        return -1;
    }

    if (queue.start(std::cerr)) {
        return -1;
    }
}

