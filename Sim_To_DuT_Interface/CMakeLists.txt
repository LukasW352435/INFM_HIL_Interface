cmake_minimum_required(VERSION 3.20)
project(Sim_To_DuT_Interface)

set(CMAKE_CXX_STANDARD 17)

add_executable(Sim_To_DuT_Interface main.cpp
        DuT_Connectors/DuTConnector.cpp DuT_Connectors/DuTConnector.h
        Sim_Communication/SimComHandler.cpp Sim_Communication/SimComHandler.h
        SimToDuTInterface.cpp SimToDuTInterface.h
        DuT_Connectors/RESTDummyConnector/RestDummyConnector.cpp
        DuT_Connectors/RESTDummyConnector/RestDummyConnector.h
        DuT_Connectors/CANConnector/CANConnector.cpp DuT_Connectors/CANConnector/CANConnector.h
        DuT_Connectors/DuTInfo.cpp DuT_Connectors/DuTInfo.h
        Events/SimEvent.cpp Events/SimEvent.h
        Utility/SharedQueue.h
        )
