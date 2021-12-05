cmake_minimum_required(VERSION 3.20)

target_sources(libs
        PRIVATE
        CANConnector.cpp
        InterfaceIndexIO.cpp
        CANConnectorConfig.cpp
        CANConnectorSendOperation.cpp
        CANConnectorReceiveOperation.cpp
        PUBLIC
        CANConnector.h
        CANConnectorConfig.h
        CANConnectorSendOperation.h
        CANConnectorReceiveOperation.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})
