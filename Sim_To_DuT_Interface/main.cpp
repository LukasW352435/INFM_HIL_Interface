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
 * @author Marco Keul
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

    /**
    
    // Create the REST connector
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

    // Test the REST connector
    auto event = sim_interface::SimEvent();
    event.operation = "Test";
    event.value = "Test";
    restDummyConnector.handleEvent(event);
    auto event2 = sim_interface::SimEvent();
    event.operation = "Indicator Right";
    event.value = "xyz";
    restDummyConnector.handleEvent(event);

    // Add the REST connector to the interface
    interface.addConnector(&restDummyConnector);

    */

    //+++++ Start CAN Connector +++++

    // Example receiveOperation Config for the 0x275 GESCHWINDIGKEIT CAN frame
    sim_interface::dut_connector::can::CANConnectorReceiveOperation receiveOperationGeschwindigkeit(
            "GESCHWINDIGKEIT",
            false,
            false
    );

    // Example receiveOperation Config for the 0x273 GPS_LOCA CAN frame
    sim_interface::dut_connector::can::CANConnectorReceiveOperation receiveOperationGPS_LOCA(
            "GPS_LOCA",
            false,
            false
    );

    // Example receiveOperation Config for the 0x274 GPS_LOCB CAN frame
    sim_interface::dut_connector::can::CANConnectorReceiveOperation receiveOperationGPS_LOCB(
            "GPS_LOCB",
            false,
            false
    );

    // Example receiveOperation Config for the 0x279 LICHTER CAN frame
    sim_interface::dut_connector::can::CANConnectorReceiveOperation receiveOperationLICHTER(
            "LICHTER",
            false,
            false
    );

    // CAN Connector Receive Config
    std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> frameToOperation = {
            {0x111, receiveOperationGeschwindigkeit},
            {0x222, receiveOperationGPS_LOCA},
            {0x333, receiveOperationGPS_LOCB},
            {0x444, receiveOperationLICHTER}
    };

    // Example sendOperation Config for the 0x275 GESCHWINDIGKEIT CAN frame
    struct bcm_timeval ival1Geschwindigkeit = {0};
    ival1Geschwindigkeit.tv_sec  = 0;
    ival1Geschwindigkeit.tv_usec = 0;

    struct bcm_timeval ival2Geschwindigkeit = {0};
    ival2Geschwindigkeit.tv_sec  = 3;
    ival2Geschwindigkeit.tv_usec = 0;

    sim_interface::dut_connector::can::CANConnectorSendOperation sendOperationGeschwindigkeit(
            0x275,
            false,
            true,
            false,
            0,
            ival1Geschwindigkeit,
            ival2Geschwindigkeit
    );

    // Example sendOperation Config for the 0x273 GPS_LOCA CAN frame
    struct bcm_timeval ival1GPS_LOCA = {0};
    ival1GPS_LOCA.tv_sec  = 0;
    ival1GPS_LOCA.tv_usec = 0;

    struct bcm_timeval ival2GPS_LOCA= {0};
    ival2GPS_LOCA.tv_sec  = 4;
    ival2GPS_LOCA.tv_usec = 0;

    sim_interface::dut_connector::can::CANConnectorSendOperation sendOperationGPS_LOCA{
        0x273,
        false,
        true,
        false,
        0,
        ival1GPS_LOCA,
        ival2GPS_LOCA
    };

    // Example sendOperation Config for the 0x274 GPS_LOCB CAN frame
    struct bcm_timeval ival1GPS_LOCB = {0};
    ival1GPS_LOCB.tv_sec  = 0;
    ival1GPS_LOCB.tv_usec = 0;

    struct bcm_timeval ival2GPS_LOCB= {0};
    ival2GPS_LOCB.tv_sec  = 4;
    ival2GPS_LOCB.tv_usec = 0;

    sim_interface::dut_connector::can::CANConnectorSendOperation sendOperationGPS_LOCB{
            0X274,
            false,
            true,
            false,
            0,
            ival1GPS_LOCB,
            ival2GPS_LOCB
    };

    // Example sendOperation Config for the 0x279 LICHTER CAN frame
    struct bcm_timeval ival1Lichter = {0};
    ival1Lichter.tv_sec  = 0;
    ival1Lichter.tv_usec = 0;

    struct bcm_timeval ival2Lichter= {0};
    ival2Lichter.tv_sec  = 4;
    ival2Lichter.tv_usec = 0;

    sim_interface::dut_connector::can::CANConnectorSendOperation sendOperationLichter{
        0x279,
        false,
        true,
        false,
        0,
        ival1Lichter,
        ival2Lichter
    };

    // CAN Connector Send Config
    std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> operationToFrame = {
            {"GESCHWINDIGKEIT", sendOperationGeschwindigkeit},
            {"GPS_LOCA", sendOperationGPS_LOCA},
            {"GPS_LOCB", sendOperationGPS_LOCB},
            {"LICHTER", sendOperationLichter}
    };

    sim_interface::dut_connector::can::CANConnectorConfig canConfig(
            "vcan0",
            "BmwCodec",
            {
                "Speed_Dynamics",
                "YawRate_Dynamics",
                "Acceleration_Dynamics",
                "Longitude_Dynamics",
                "Latitude_Dynamics",
                "Position_Z-Coordinate_DUT",
                "Heading_Dynamics"
                },
            frameToOperation,
            operationToFrame,
            {},
            false);

    // Create a new CAN Connector and add it to the interface
    sim_interface::dut_connector::can::CANConnector canConnector(interface.getQueueDuTToSim(), canConfig);
    interface.addConnector(&canConnector);

    // Test the CAN Connector
    interface.getQueueSimToInterface()->push(sim_interface::SimEvent("Speed_Dynamics",2.0,"TEST"));
    interface.getQueueSimToInterface()->push(sim_interface::SimEvent("Latitude_Dynamics",3.0,"TEST"));
    interface.getQueueSimToInterface()->push(sim_interface::SimEvent("Latitude_Dynamics",4.0,"TEST"));

    //+++++ End CAN Connector +++++

    //std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    //std::thread simComHandlerThread(&sim_interface::SimComHandler::run, &simComHandler);
    //simComHandlerThread.detach();

    // Start interface to receive/send events
    interface.run();

    std::cin.get();
    DuTLogger::logMessage("Shut down application", LOG_LEVEL::INFO);
    return 0;
}
