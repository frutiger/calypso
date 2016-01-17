# CMakeLists.txt

cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Wall -Wextra -Werror")

add_library(hauberk standalones/hauberk/hauberk_bufferutil.cpp
                    standalones/hauberk/hauberk_dnsutil.cpp
                    standalones/hauberk/hauberk_ethernetutil.cpp
                    standalones/hauberk/hauberk_internetutil.cpp
                    standalones/hauberk/hauberk_internetaddressutil.cpp
                    standalones/hauberk/hauberk_loopbackutil.cpp
                    standalones/hauberk/hauberk_socketaddress.cpp
                    standalones/hauberk/hauberk_udputil.cpp)
target_include_directories(hauberk PUBLIC standalones/hauberk)

add_library(trammel standalones/trammel/trammel_address.cpp
                    standalones/trammel/trammel_duplex.cpp
                    standalones/trammel/trammel_interface.cpp
                    standalones/trammel/trammel_interfacesutil.cpp
                    standalones/trammel/trammel_iterator.cpp
                    standalones/trammel/trammel_list.cpp)
target_include_directories(trammel PUBLIC standalones/trammel)
target_link_libraries(trammel hauberk maxwell -lpcap)

add_library(conduit standalones/conduit/conduit_listener.cpp
                    standalones/conduit/conduit_resolver.cpp
                    standalones/conduit/conduit_argumentparser.cpp)
target_include_directories(conduit PUBLIC standalones/conduit)
target_link_libraries(conduit trammel maxwell)

add_library(maxwell standalones/maxwell/maxwell_queue.cpp)
target_include_directories(maxwell PUBLIC standalones/maxwell)

add_executable(calypso standalones/calypso/calypso.m.cpp)
target_link_libraries(calypso maxwell conduit trammel hauberk)

