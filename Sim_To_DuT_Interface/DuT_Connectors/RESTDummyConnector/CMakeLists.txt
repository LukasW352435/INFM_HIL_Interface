cmake_minimum_required(VERSION 3.20)

set(BUILD_TESTS OFF CACHE BOOL "Build unit tests.")
add_subdirectory(restbed)

target_sources(libs
        PRIVATE
        ReceiveEndpoint.cpp
        ReceiveEndpoint.h
        RESTDummyConnector.cpp
        PUBLIC
        RESTDummyConnector.h
        RESTConfig.h)

find_package(CURL REQUIRED)

target_link_libraries(libs PUBLIC restbed-static CURL::libcurl pthread)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR} restbed/source)
