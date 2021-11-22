cmake_minimum_required(VERSION 3.20)

target_sources(libs
        PRIVATE
        CANConnector.cpp
        InterfaceIndexIO.cpp
        PUBLIC
        CANConnector.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})
