cmake_minimum_required(VERSION 3.20)

add_subdirectory(CANConnector)
add_subdirectory(RESTDummyConnector)
if(V2X_ENABLED)
add_subdirectory(V2XConnector)
endif()


target_sources(libs
        PRIVATE
        DuTConnector.cpp
        ConnectorInfo.cpp
        PUBLIC
        DuTConnector.h
        ConnectorInfo.h
        ConnectorConfig.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})