cmake_minimum_required(VERSION 3.20)
project(Sim_To_DuT_Interface)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sim_To_DuT_Interface main.cpp
        DuT_Connectors/DuT_Connector.cpp DuT_Connectors/DuT_Connector.h
        Sim_Communication/Sim_Com_Handler.cpp Sim_Communication/Sim_Com_Handler.h
        Sim_To_DuT_Interface.cpp Sim_To_DuT_Interface.h
        DuT_Connectors/REST_Dummy_Connector/Rest_Dummy_Connector.cpp
        DuT_Connectors/REST_Dummy_Connector/Rest_Dummy_Connector.h
        DuT_Connectors/CAN_Connector/CAN_Connector.cpp DuT_Connectors/CAN_Connector/CAN_Connector.h
        DuT_Connectors/DuT_Info.cpp DuT_Connectors/DuT_Info.h)
