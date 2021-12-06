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
 * @author Matthias Bank
 * // TODO add all authors
 * @version 1.0
 */

// Project includes
#include "SimToDuTInterface.h"
#include "Sim_Communication/SimComHandler.h"
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"
#include "DuT_Connectors/CANConnector/CANConnector.h"
#include "DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "DuTLogger/DuTLogger.h"
#include "SystemConfig.h"

// System includes
#include <thread>
#include <iostream>


int main() {
    // System config
    sim_interface::SystemConfig systemConfig;
    std::string configPath = std::filesystem::canonical("/proc/self/exe").parent_path().string();
    sim_interface::SystemConfig::loadFromFile(configPath + "/SystemConfig.xml", systemConfig, true);

    // initialize the logger
    DuTLogger::initializeLogger(systemConfig.loggerConfig);

    DuTLogger::logMessage("Start Application", LOG_LEVEL::INFO);


    // Create interface
    sim_interface::SimToDuTInterface interface;

    // Create simComHandler
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface(), systemConfig);

    // Init interface with SimComHandler
    interface.setSimComHandler(&simComHandler);

    // Create DuT Devices
    sim_interface::dut_connector::rest_dummy::RESTConnectorConfig config("http://localhost:9090",
                                                                         "http://172.17.0.1",
                                                                         9091,
                                                                         {"Test", "Angle",
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
                                                                          "origin"},
                                                                         {{"Test", 1000}},
                                                                         true);

    sim_interface::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(),
                                                                                    config);
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

    //+++++ Start CAN Connector +++++

    // CAN receive operation without a mask
    sim_interface::dut_connector::can::CANConnectorReceiveOperation recvOpCan1(
            "Hazard",
            false,
            false
    );

    // CANFD receive operation mask
    int mask1Len = 1;
    __u8 mask1[1] = {0xFF};

    sim_interface::dut_connector::can::CANConnectorReceiveOperation recvOpCanfd1(
            "Brake",
            true,
            true,
            mask1Len,
            mask1
    );

    // CAN non-cyclic send operation
    sim_interface::dut_connector::can::CANConnectorSendOperation sendOpCan1(
            0x789,
            false,
            false
    );

    // CANFD cyclic send operation
    struct bcm_timeval ival1 = {0};
    ival1.tv_sec = 1;
    ival1.tv_usec = 0;

    struct bcm_timeval ival2 = {0};
    ival2.tv_sec = 3;
    ival2.tv_usec = 0;

    sim_interface::dut_connector::can::CANConnectorSendOperation sendOpCyclicCanfd1(
            0x9AB,
            true,
            true,
            true,
            10,
            ival1,
            ival2
    );

    // CAN Connector Receive Config
    std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> frameToOperation = {
            {0x123, recvOpCan1},
            {0x456, recvOpCanfd1}
    };

    // CAN Connector Send Config
    std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> operationToFrame = {
            {"Speed", sendOpCan1},
            {"Blink", sendOpCyclicCanfd1}
    };

    sim_interface::dut_connector::can::CANConnectorConfig canConfig(
            "vcan0",
            {"Speed", "Blink", "Hazard", "Brake"},
            frameToOperation,
            operationToFrame,
            {},
            false);

    // Create a new CAN Connector and add it to the interface
    sim_interface::dut_connector::can::CANConnector canConnector(interface.getQueueDuTToSim(), canConfig);
    interface.addConnector(&canConnector);

    // Test the CAN Connector
    auto canEvent = sim_interface::SimEvent();
    canEvent.operation = "Test";
    canEvent.value = "Value";
    canConnector.handleEventSingle(canEvent);

    //+++++ End CAN Connector +++++

    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    std::thread simComHandlerThread(&sim_interface::SimComHandler::run, &simComHandler);
    simComHandlerThread.detach();

    // Start interface to receive/send events
    interface.run();

    std::cin.get();
    DuTLogger::logMessage("Shut down application", LOG_LEVEL::INFO);
    return 0;
}
