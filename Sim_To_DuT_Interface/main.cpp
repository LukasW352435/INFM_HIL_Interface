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
#include <set>
#include <map>
#include <string>
#include "SimToDuTInterface.h"
#include <thread>
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"
#include "DuT_Connectors/CANConnector/CANConnector.h"
#include "DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "DuT_Connectors/CANConnector/CANConnectorSendOperation.h"
#include "DuT_Connectors/CANConnector/CANConnectorReceiveOperation.h"
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
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface(), socketSimAddressSub, context_sub,
                                               socketSimAddressPub, context_pub);

    interface.setSimComHandler(&simComHandler);


    // Create a new CAN Connector config

    std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> frameToOperation = {};
    std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> operationToFrame = {};

    sim_interface::dut_connector::can::CANConnectorConfig canConfig(
            "vcan0",
            {"Speed", "Blink"},
            frameToOperation,
            operationToFrame,
            {},
            false
            );

    // Create a new CAN Connector and add it to the interface
    sim_interface::dut_connector::can::CANConnector canConnector(interface.getQueueDuTToSim(), canConfig);
    interface.addConnector(&canConnector);

    // Test the CAN Connector
    auto canEvent = sim_interface::SimEvent();
    canEvent.operation = "Test";
    canEvent.value = "Value";
    canConnector.handleEvent(canEvent);

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
