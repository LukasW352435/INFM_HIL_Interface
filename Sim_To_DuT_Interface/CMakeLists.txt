cmake_minimum_required(VERSION 3.20)
project(Sim_To_DuT_Interface)

set(CMAKE_CXX_STANDARD 17)

add_library(libs "")

add_subdirectory(DuT_Connectors)

add_executable(${PROJECT_NAME} main.cpp
        SimToDuTInterface.cpp SimToDuTInterface.h
        DuT_Connectors/DuTConnector.cpp DuT_Connectors/DuTConnector.h
        DuT_Connectors/DuTInfo.cpp DuT_Connectors/DuTInfo.h
        Events/SimEvent.cpp Events/SimEvent.h
        Sim_Communication/SimComHandler.cpp Sim_Communication/SimComHandler.h
        Utility/SharedQueue.h)
target_link_libraries(${PROJECT_NAME} libs)