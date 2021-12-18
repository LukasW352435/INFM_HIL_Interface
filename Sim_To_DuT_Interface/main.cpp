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
 * @author Franziska Ihrler
 * @author Matthias Bank
 * @author Fabian Andre Genes
 * @author Thanaancheyan Thavapalan
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
#include "Interface_Logger/InterfaceLogger.h"
#include "DuT_Connectors/V2XConnector/V2XConnectorConfig.h"
#include "DuT_Connectors/V2XConnector/V2XConnector.h"
#include "SystemConfig.h"

// System includes
#include <thread>
#include <iostream>
#include "Utility/ConfigSerializer.h"
#include <boost/archive/xml_oarchive.hpp>
#include <typeinfo>
#include <memory>
#include <boost/archive/text_oarchive.hpp>


int main() {

    // System config
    sim_interface::SystemConfig systemConfig;
    std::string configPath = std::filesystem::canonical("/proc/self/exe").parent_path().string();
    sim_interface::SystemConfig::loadFromFile(configPath + "/SystemConfig.xml", systemConfig, true);

    // initialize the logger
    sim_interface::InterfaceLogger::initializeLogger(systemConfig.loggerConfig);
    sim_interface::InterfaceLogger::logMessage("Start Application", sim_interface::LOG_LEVEL::INFO);

    // Create interface
    sim_interface::SimToDuTInterface interface;

    // Create simComHandler
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface(), systemConfig);

    // Init interface with SimComHandler
    interface.setSimComHandler(&simComHandler);
    std::vector<sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *> RESTConnectorVec;
    std::vector<sim_interface::dut_connector::can::CANConnectorConfig *> CanConnectorVec;
    std::vector<sim_interface::dut_connector::v2x::V2XConnectorConfig *> V2XConnectorVec;
    std::cout << "Laenge des REST-Vektor vor dem Hinzufuegen: " << RESTConnectorVec.size() << std::endl;
    std::cout << "Laenge des CAN-Vektor vor dem Hinzufuegen: " << CanConnectorVec.size() << std::endl;
    std::cout << "Laenge des CAN-Vektor vor dem Hinzufuegen: " << V2XConnectorVec.size() << std::endl;
    simComHandler.getConfig(&RESTConnectorVec, &CanConnectorVec, &V2XConnectorVec);
    std::cout << "Laenge des CAN-Vektor nach dem Hinzufuegen: " << CanConnectorVec.size() << std::endl;
    std::cout << "Laenge des REST-Vektor nach dem Hinzufuegen: " << RESTConnectorVec.size() << std::endl;
    std::cout << "Laenge des CAN-Vektor vor dem Hinzufuegen: " << V2XConnectorVec.size() << std::endl;

    if (!RESTConnectorVec.empty()) {
        for (auto const &RESTconfig: RESTConnectorVec) {

        }
    }
    sim_interface::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(),
                                                                                    *RESTConnectorVec.at(0));
    interface.addConnector(&restDummyConnector);
    //Funkt im IF nicht

    // if(!CanConnectorVec.empty() ) {
    // for (auto const & CanConfig: CanConnectorVec) {
    sim_interface::dut_connector::can::CANConnector canConnector(interface.getQueueDuTToSim(), *CanConnectorVec.at(0));
    // }
    //  }

    if (!V2XConnectorVec.empty()) {
        for (auto const &V2XConfig: V2XConnectorVec) {
            //  sim_interface::dut_connector::v2x::V2XConnector V2XConnector(interface.getQueueDuTToSim(), *V2XConnectorVec.at(0));
            //  interface.addConnector(&V2XConnector);
        }
    }

    // Create DuT Devices


    // Create the REST connector
    /*
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
*/
    //  sim_interface::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(),
    //                                                                                  config);

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


    //V2x Connector

    sim_interface::dut_connector::v2x::V2XConnectorConfig v2xconfig("veth0", 0x0000);

/*
    sim_interface::dut_connector::v2x::V2XConnector v2xConnector(interface.getQueueDuTToSim(), v2xconfig);
    interface.addConnector(&v2xConnector);
    //Testing V2x
    std::stringstream ss;
    boost::archive::text_oarchive ar(ss);
    std::string sourceMAC = "aa:bb:cc:dd:ee:ff";
    std::string destinationMAC = "11:22:33:44:55:66";
    int payload_length = 2;
    std::vector<unsigned char> payload = {0x12, 0x34};
    ar << sourceMAC;
    ar << destinationMAC;
    ar << payload_length;
    ar << payload[0];
    ar << payload[1];
    std::string archive = ss.str();
    const sim_interface::SimEvent e("V2X", ss.str(), "Simulation");
    v2xConnector.handleEventSingle(e);
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
    ival1Geschwindigkeit.tv_sec = 0;
    ival1Geschwindigkeit.tv_usec = 0;

    struct bcm_timeval ival2Geschwindigkeit = {0};
    ival2Geschwindigkeit.tv_sec = 3;
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
    ival1GPS_LOCA.tv_sec = 0;
    ival1GPS_LOCA.tv_usec = 0;

    struct bcm_timeval ival2GPS_LOCA = {0};
    ival2GPS_LOCA.tv_sec = 4;
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
    ival1GPS_LOCB.tv_sec = 0;
    ival1GPS_LOCB.tv_usec = 0;

    struct bcm_timeval ival2GPS_LOCB = {0};
    ival2GPS_LOCB.tv_sec = 4;
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
    ival1Lichter.tv_sec = 0;
    ival1Lichter.tv_usec = 0;

    struct bcm_timeval ival2Lichter = {0};
    ival2Lichter.tv_sec = 4;
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
            {"GPS_LOCA",        sendOperationGPS_LOCA},
            {"GPS_LOCB",        sendOperationGPS_LOCB},
            {"LICHTER",         sendOperationLichter}
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

    sim_interface::dut_connector::can::CANConnectorConfig canConfigTest(
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

    // sim_interface::dut_connector::can::CANConnectorConfig* canConfigTestPointer = &canConfig;
    // sim_interface::ConfigSerializer::serialize("JAMOIN.xml","conn", canConfigTestPointer);
    // Create a new CAN Connector and add it to the interface
    //   sim_interface::dut_connector::can::CANConnector canConnector(interface.getQueueDuTToSim(), canConfig);
    //interface.addConnector(&canConnector);

    // Test the CAN Connector
    //interface.getQueueSimToInterface()->push(sim_interface::SimEvent("Speed_Dynamics",2.0,"TEST"));
    //interface.getQueueSimToInterface()->push(sim_interface::SimEvent("Latitude_Dynamics",3.0,"TEST"));
    //interface.getQueueSimToInterface()->push(sim_interface::SimEvent("Latitude_Dynamics",4.0,"TEST"));

    //+++++ End CAN Connector +++++


    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    std::thread simComHandlerThread(&sim_interface::SimComHandler::run, &simComHandler);
    simComHandlerThread.detach();

    // Start interface to receive/send events
    interface.run();

    std::cin.get();
    sim_interface::InterfaceLogger::logMessage("Shut down application", sim_interface::LOG_LEVEL::INFO);
    return 0;
}
