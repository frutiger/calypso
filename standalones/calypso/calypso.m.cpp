// calypso.m.cpp

#include <conduit_argumentparser.h>
#include <conduit_listener.h>
#include <maxwell_queue.h>

#include <array>
#include <iostream>
#include <vector>

namespace {

static const std::uint64_t TEN_MS_AS_NS = 10 * 1000 * 1000;

}  // close unnamed namespace

int main(int argc, char **argv)
{
    conduit::ArgumentParser::InterfaceAddresses interfaces;
    std::vector<std::string>                    arguments(argv + 1,
                                                          argv + argc);
    if (conduit::ArgumentParser::toInterfaces(std::cerr,
                                              &interfaces,
                                              argv[0],
                                              arguments)) {
        return -1;
    }

    maxwell::Queue queue(TEN_MS_AS_NS);
    if (queue.create(std::cerr)) {
        return -1;
    }

    conduit::Listener listener(interfaces[0].first,
                               interfaces[0].second);
    if (listener.activate(std::cerr, queue)) {
        return -1;
    }

    if (queue.start(std::cerr)) {
        return -1;
    }
}

