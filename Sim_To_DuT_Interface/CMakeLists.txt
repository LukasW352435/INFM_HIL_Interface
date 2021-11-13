cmake_minimum_required(VERSION 3.20)
project(Sim_To_DuT_Interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

add_library(libs "")

add_subdirectory(DuTLogger/quill)
    
add_subdirectory(DuT_Connectors)
add_executable(${PROJECT_NAME} main.cpp
        SimToDuTInterface.cpp SimToDuTInterface.h
        Events/SimEvent.cpp Events/SimEvent.h
        Sim_Communication/SimComHandler.cpp Sim_Communication/SimComHandler.h
        Utility/SharedQueue.h
        DuTLogger/DuTLogger.h
        DuTLogger/DuTLogger.cpp)
        
target_link_libraries(${PROJECT_NAME} libs zmq quill)