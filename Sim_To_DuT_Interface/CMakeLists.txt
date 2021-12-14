cmake_minimum_required(VERSION 3.20)
project(Sim_To_DuT_Interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_C_FLAGS -pthread)

add_library(libs "")

add_subdirectory(DuTLogger)

add_subdirectory(DuT_Connectors)

add_executable(${PROJECT_NAME} main.cpp
        SimToDuTInterface.cpp SimToDuTInterface.h
        Events/SimEvent.cpp Events/SimEvent.h
        Sim_Communication/SimComHandler.cpp Sim_Communication/SimComHandler.h
        Utility/SharedQueue.h
        Utility/PeriodicTimer.cpp Utility/PeriodicTimer.h
        DuTLogger/DuTLogger.cpp DuTLogger/DuTLogger.h
        DuTLogger/DuTLoggerConfig.h
        SystemConfig.h)


if (CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 9)
    target_link_libraries(${PROJECT_NAME} libs zmq quill boost_serialization)
else()
    target_link_libraries(${PROJECT_NAME} libs zmq quill boost_serialization stdc++fs)
endif()

