cmake_minimum_required(VERSION 3.20)

target_sources(libs
        PRIVATE
        EthernetPacket.cpp
        EthernetPacket.h
        V2XConnector.cpp
        PUBLIC
        V2XConnector.h
        V2XConnectorConfig.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})