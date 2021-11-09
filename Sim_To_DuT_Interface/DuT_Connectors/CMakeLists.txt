cmake_minimum_required(VERSION 3.20)

add_subdirectory(CANConnector)
add_subdirectory(RESTDummyConnector)

target_sources(libs
        PRIVATE
        DuTConnector.cpp
        DuTInfo.cpp
        PUBLIC
        DuTConnector.h
        DuTInfo.h
        ConnectorConfig.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})