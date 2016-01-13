# CMakeLists.txt

cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -Wextra")

add_library(hauberk standalones/hauberk/hauberk_bufferutil.cpp
                    standalones/hauberk/hauberk_internet.cpp
                    standalones/hauberk/hauberk_internetaddressutil.cpp
                    standalones/hauberk/hauberk_loopback.cpp
                    standalones/hauberk/hauberk_socketaddress.cpp
                    standalones/hauberk/hauberk_udp.cpp)
target_include_directories(hauberk PUBLIC standalones/hauberk)
target_compile_features(hauberk PUBLIC cxx_strong_enums)

add_library(trammel standalones/trammel/trammel_address.cpp
                    standalones/trammel/trammel_capture.cpp
                    standalones/trammel/trammel_interface.cpp
                    standalones/trammel/trammel_interfacesutil.cpp
                    standalones/trammel/trammel_iterator.cpp
                    standalones/trammel/trammel_list.cpp
                    standalones/trammel/trammel_send.cpp)
target_include_directories(hauberk PUBLIC standalones/trammel)
target_compile_features(trammel PUBLIC cxx_alias_templates)
target_link_libraries(trammel hauberk -lpcap)

add_library(conduit standalones/conduit/conduit_listener.cpp
                    standalones/conduit/conduit_resolver.cpp
                    standalones/conduit/conduit_argumentparser.cpp)
target_include_directories(conduit PUBLIC standalones/conduit)
target_link_libraries(conduit trammel maxwell)

add_library(maxwell standalones/maxwell/maxwell_eventhandler.cpp
                    standalones/maxwell/maxwell_queue.cpp)
target_include_directories(maxwell PUBLIC standalones/maxwell)

add_executable(calypso standalones/calypso/calypso.m.cpp)
target_link_libraries(calypso maxwell conduit trammel hauberk)

