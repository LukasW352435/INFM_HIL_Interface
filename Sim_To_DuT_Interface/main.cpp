/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
 *
 * This file is part of "Sim To DuT Interface".
 *
 * "Sim To DuT Interface" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sim To DuT Interface" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Lukas Wagenlehner
 * @author Michael Schmitz
 * // TODO add all authors
 * @version 1.0
 */

#include <iostream>
#include "SimToDuTInterface.h"
#include <thread>
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConfig.h"
#include "DuT_Connectors/CANConnector/CANConnector.h"
#include "DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "Utility/SharedQueue.h"
#include "Sim_Communication/SimComHandler.h"
#include "DuTLogger/DuTLogger.h"

int main() {
    DuTLogger::logMessage("Start Application", LOG_LEVEL::INFO);
    
    // Create interface
    sim_interface::SimToDuTInterface interface;
    // Create simComHandler
    std::string socketSimAddressSub = "tcp://localhost:7777";
    zmq::context_t context_sub(1);
    std::string socketSimAddressPub = "tcp://*:7778";
    zmq::context_t context_pub(1);
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface(), socketSimAddressSub, context_sub, socketSimAddressPub , context_pub);

    interface.setSimComHandler(&simComHandler);

    // Create DuT Devices
    sim_interface::dut_connector::rest_dummy::RESTConfig config;
    config.baseUrlDuT = "http://localhost:9090";
    config.baseCallbackUrl = "http://172.17.0.1";
    config.port = 9091;
    config.operations = {"Test","Angle",
    "Acceleration",
    "Decel",
    "Distance",
    "Height",
    "LaneID",
    "LaneIndex",
    "LanePosition",
    "Length",
    "Position_X-Coordinate",
    "Position_Y-Coordinate",
    "Position_Z-Coordinate",
    "RoadID",
    "RouteIndex",
    "Signals",
    "Speed",
    "Width",
    "current",
    "origin"};
    config.periodicTimerEnabled = true;
    config.periodicOperations = {{ "Test", 1000 }};

    sim_interface::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(), config);
    /*
    auto event = sim_interface::SimEvent();
    event.operation = "Test";
    event.value = "Test";
    restDummyConnector.handleEvent(event);
    auto event2 = sim_interface::SimEvent();
    event.operation = "Indicator Right";
    event.value = "xyz";
    restDummyConnector.handleEvent(event);
    */
  
    interface.addConnector(&restDummyConnector);

    // Create a new CAN Connector config
    sim_interface::dut_connector::can::CANConnectorConfig canConfig;
    canConfig.interfaceName = "vcan0";

    // Create a new CAN Connector and add it to the interface
    sim_interface::dut_connector::can::CANConnector canConnector(interface.getQueueDuTToSim(), canConfig);
    interface.addConnector(&canConnector);

    // Test the CAN Connector
    auto canEvent = sim_interface::SimEvent();
    canEvent.operation = "Test";
    canEvent.value     = "Value";
    canConnector.handleEvent(canEvent);

    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    std::thread simComHandlerThread (&sim_interface::SimComHandler::run, &simComHandler);
    simComHandlerThread.detach();

    // Start interface to receive/send events
    interface.run();
    
    std::cin.get();
    DuTLogger::logMessage("Shut down application", LOG_LEVEL::INFO);
    return 0;
}
