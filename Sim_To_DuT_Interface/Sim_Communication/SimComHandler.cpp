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
 * @author Fabian Andre Genes
 * @author Thanaancheyan Thavapalan
 * @version 1.0
 */

#include "SimComHandler.h"
#include "../Utility/ConfigSerializer.h"
#include "../DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "../DuT_Connectors/CANConnector/CANConnector.h"
#include "../DuT_Connectors/V2XConnector/V2XConnector.h"


#include <exception>
#include <string>
#include <thread>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>

#include <boost/algorithm/string.hpp>


namespace sim_interface {


    /**
   * Global Variable because: socket doesn't work, if created in constructor
   * @param context_sub: Defines the context of the zmq socket
   */
    zmq::context_t context_sub(1);

    SimComHandler::SimComHandler(SimToDuTInterface *interface, const SystemConfig &config)
            : interface(interface), socketSimPub_(context_sub, zmq::socket_type::pub),
              socketSimSub_(context_sub, zmq::socket_type::sub),
              socketSimSubConfig_(context_sub, zmq::socket_type::sub) {

        // zmq Subscriber
        socketSimAddressSub = config.socketSimAddressSub;
        // zmq Publisher
        socketSimAddressPub = config.socketSimAddressPub;
        // zmq Reciver Config
        socketSimAddressReceiverConfig = config.socketSimAddressReciverConfig;
        zmq::context_t context_recConfig(1);
        // Config Sockets
        socketSimSub_.set(zmq::sockopt::subscribe, "");
        socketSimSubConfig_.set(zmq::sockopt::subscribe, "");
        //Open the connections
        InterfaceLogger::logMessage("Connecting to subscriber (simulation data): " + socketSimAddressSub,
                                    LOG_LEVEL::INFO);
        InterfaceLogger::logMessage("Connecting to subscriber (config): " + socketSimAddressReceiverConfig,
                                    LOG_LEVEL::INFO);
        InterfaceLogger::logMessage("Binding to publisher (events interface): " + socketSimAddressPub, LOG_LEVEL::INFO);
        socketSimPub_.bind(socketSimAddressPub);
        socketSimSub_.connect(socketSimAddressSub);
        socketSimSubConfig_.connect(socketSimAddressReceiverConfig);

    }

    SimComHandler::connectorType SimComHandler::resolveConnectorTypeForSwitch(std::string connectorTypeS) {
        if (connectorTypeS == "RESTDummyConnector") return RESTDummyConnector;
        if (connectorTypeS == "CANConnector") return CANConnector;
        if (connectorTypeS == "V2XConnector") return V2XConnector;
        return Invalid_Connector;

    }

    void SimComHandler::getConfig() {
        zmq::message_t replyConfig;
        try {
            socketSimSubConfig_.recv(replyConfig, zmq::recv_flags::none);
            InterfaceLogger::logMessage("Received Config: ", LOG_LEVEL::INFO);
        } catch (zmq::error_t &cantReceive) {
            InterfaceLogger::logMessage(cantReceive.what(), LOG_LEVEL::ERROR);
            unbind();
            disconnect();
        }

        const char *bufConfig = static_cast<const char *>(replyConfig.data());
        boost::property_tree::ptree tree;
        std::ostringstream convertedSimConfigBuffer;
        convertedSimConfigBuffer << bufConfig;
        std::string SimConfigString = convertedSimConfigBuffer.str();
        std::stringstream stringStream(SimConfigString);

        // Parse the XML into the property tree.
        try {

            boost::property_tree::read_xml(stringStream, tree);
            InterfaceLogger::logMessage("XML-Config was in the correct format: ", LOG_LEVEL::INFO);
        } catch (std::exception &e) {
            InterfaceLogger::logMessage(e.what(), LOG_LEVEL::ERROR);
            unbind();
            disconnect();
        }
        std::string connectorTypes;
        auto xmlWriterSettings = boost::property_tree::xml_writer_make_settings<std::string>(' ', 4);
        for (boost::property_tree::ptree::value_type &connector: tree.get_child("connectors")) {
            connectorTypes = connector.second.get<std::string>("<xmlattr>.classType");


            switch (resolveConnectorTypeForSwitch(connectorTypes)) {
                case RESTDummyConnector: {

                    try {
                        std::stringstream restXMLStringStream;
                        boost::property_tree::xml_parser::write_xml(restXMLStringStream, connector.second,
                                                                    xmlWriterSettings);
                        std::string restXMLString = restXMLStringStream.str();

                        boost::algorithm::replace_all(restXMLString, R"(<?xml version="1.0" encoding="utf-8"?>)", "");
                        sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *restConnectorConfig;
                        std::istringstream restXMLIStringStream(restXMLString);
                        ConfigSerializer::deserialize(restXMLIStringStream, "conn", &restConnectorConfig);

                        auto restConnector = std::make_shared<dut_connector::rest_dummy::RESTDummyConnector>(
                                interface->getQueueDuTToSim(), *restConnectorConfig);
                        interface->addConnector(restConnector);
                    }

                    catch (std::exception &e) {
                        InterfaceLogger::logMessage(e.what(), LOG_LEVEL::ERROR);
                    }


                    break;
                }
                case CANConnector: {

                    try {

                        std::stringstream canXMLStringStream;
                        boost::property_tree::xml_parser::write_xml(canXMLStringStream, connector.second,
                                                                    xmlWriterSettings);
                        std::string canXMLString = canXMLStringStream.str();
                        boost::algorithm::trim(canXMLString);
                        boost::algorithm::replace_all(canXMLString, R"(<?xml version="1.0" encoding="utf-8"?>)", "");

                        //Workaround because frameToOperation & OperationToframe need class_id_reference to work

                        boost::algorithm::replace_all(canXMLString, R"(<frameToOperation>)",
                                                      R"(<frameToOperation class_id="2">)");
                        boost::algorithm::replace_all(canXMLString, R"(<operationToFrame>)",
                                                      R"(<operationToFrame class_id="5">)");
                        boost::algorithm::replace_all(canXMLString, R"(<itemOperationToFrame>)",
                                                      R"(<itemOperationToFrame class_id_reference="6">)");

                        boost::algorithm::replace_first(canXMLString,
                                                        R"(<itemOperationToFrame class_id_reference="6">)",
                                                        R"(<itemOperationToFrame class_id="6">)");

                        boost::algorithm::replace_all(canXMLString, R"(<itemFrameToOperation>)",
                                                      R"(<itemFrameToOperation class_id_reference="3">)");

                        boost::algorithm::replace_first(canXMLString, R"(<itemFrameToOperation class_id_reference=3">)",
                                                        R"(<itemFrameToOperation class_id="3">)");
                        boost::algorithm::replace_first(canXMLString, R"(<ival1>)",
                                                        R"(<ival1 class_id="8" tracking_level="0">)");
                        boost::algorithm::replace_all(canXMLString, R"(<CANConnectorReceiveOperation>)",
                                                      R"(<CANConnectorReceiveOperation class_id_reference="4">)");
                        boost::algorithm::replace_first(canXMLString,
                                                        R"(<CANConnectorReceiveOperation class_id_reference="6">)",
                                                        R"(<CANConnectorReceiveOperation class_id="4">)");

                        boost::algorithm::replace_all(canXMLString, R"(<CANConnectorSendOperation>)",
                                                      R"(<CANConnectorSendOperation class_id_reference="7">)");
                        boost::algorithm::replace_first(canXMLString,
                                                        R"(<CANConnectorSendOperation class_id_reference="7">)",
                                                        R"(<CANConnectorSendOperation class_id="7">)");

                        std::istringstream canXMLIStringStream(canXMLString);
                        sim_interface::dut_connector::can::CANConnectorConfig *canConnectorConfig;
                        ConfigSerializer::deserialize(canXMLIStringStream, "conn", &canConnectorConfig);

                        auto CANConnector = std::make_shared<dut_connector::can::CANConnector>(
                                interface->getQueueDuTToSim(), *canConnectorConfig);
                        interface->addConnector(CANConnector);
                    }

                    catch (std::exception &e) {
                        InterfaceLogger::logMessage(e.what(), LOG_LEVEL::ERROR);
                    }

                    break;
                }

                case V2XConnector: {

                    try {
                        std::stringstream v2xXMLStringStream;
                        boost::property_tree::xml_parser::write_xml(v2xXMLStringStream, connector.second,
                                                                    xmlWriterSettings);
                        std::string v2xXMLString = v2xXMLStringStream.str();

                        boost::algorithm::replace_all(v2xXMLString, R"(<?xml version="1.0" encoding="utf-8"?>)", "");
                        //Fix v2xXMLString with class_id
                        boost::algorithm::replace_first(v2xXMLString, R"(<conn>)", R"(<conn class_id="0">)");

                        std::istringstream v2xXMLIStringStream(v2xXMLString);
                        sim_interface::dut_connector::v2x::V2XConnectorConfig *V2XConnectorConfig;
                        ConfigSerializer::deserialize(v2xXMLIStringStream, "conn", &V2XConnectorConfig);

                        auto V2XConnector = std::make_shared<dut_connector::v2x::V2XConnector>(
                                interface->getQueueDuTToSim(), *V2XConnectorConfig);

                        interface->addConnector(V2XConnector);
                    }

                    catch (std::exception &e) {
                        InterfaceLogger::logMessage(e.what(), LOG_LEVEL::ERROR);
                    }

                    break;
                }
                default: {

                    break;
                }


            }
        }
        std::stringstream logss;
        logss << *interface;
        InterfaceLogger::logMessage("Received configs, the following connectors were created:", LOG_LEVEL::INFO);
        for (std::string line; std::getline(logss, line);) {
            InterfaceLogger::logMessage(line, LOG_LEVEL::INFO);
        }

        std::string receivedString = "Config received";
        socketSimPub_.send(zmq::message_t(receivedString), zmq::send_flags::dontwait);

    }


    void SimComHandler::receive() {


        while (stopThread) {
            zmq::message_t replySimData;
            try {
                socketSimSub_.recv(replySimData, zmq::recv_flags::none);
                InterfaceLogger::logMessage("Receiving data ", LOG_LEVEL::INFO);

            } catch (zmq::error_t &cantReceive) {
                InterfaceLogger::logMessage(cantReceive.what(), LOG_LEVEL::ERROR);
                unbind();
                disconnect();
            }

            const char *bufSimData = static_cast<const char *>(replySimData.data());

            std::string inputSimData(bufSimData, replySimData.size());
            std::istringstream archiveStreamSimData(inputSimData);
            boost::archive::text_iarchive archiveSimData(archiveStreamSimData);
            std::map<std::string, boost::variant<int, double, std::string>> receiveMapSimData;

            try {
                archiveSimData >> receiveMapSimData;
            } catch (boost::archive::archive_exception &ex) {
                InterfaceLogger::logMessage("Archive exception during deserializing: ", LOG_LEVEL::ERROR);
                InterfaceLogger::logMessage(ex.what(), LOG_LEVEL::ERROR);

            } catch (int e) {
                InterfaceLogger::logMessage(" Exception:" + fmt::to_string(e), LOG_LEVEL::ERROR);

            }


            for (auto const &element: receiveMapSimData) {
                SimEvent event(element.first, element.second, "Simulation Traci");
                sendEventToInterface(event);
            }
        }
    }

    void SimComHandler::sendEventToSim(const SimEvent &simEvent) {


        std::stringstream logSimEvent;
        logSimEvent << simEvent;

        std::map<std::string, boost::variant<int, double, std::string>> simEventMap;
        simEventMap["Operation"] = simEvent.operation;
        simEventMap["Value"] = boost::apply_visitor(EventVisitor(), simEvent.value);
        simEventMap["Origin"] = simEvent.origin;
        simEventMap["Current"] = simEvent.current;

        //serialize map
        std::ostringstream simEventsStringStream;
        boost::archive::text_oarchive archiveSimEvent(simEventsStringStream);
        archiveSimEvent << simEventMap;
        std::string simEventData = simEventsStringStream.str();

        zmq::message_t msgToSend(simEventData);
        socketSimPub_.send(msgToSend, zmq::send_flags::none);
        InterfaceLogger::logMessage("Async sending of events" + logSimEvent.str(), LOG_LEVEL::INFO);
    }

    void SimComHandler::sendEventToInterface(const SimEvent &simEvent) {
        InterfaceLogger::logEvent(simEvent);
        interface->getQueueSimToInterface()->push(simEvent);
    }


    void SimComHandler::unbind() {
        socketSimPub_.unbind(socketSimAddressPub);
        InterfaceLogger::logMessage("Unbinding to publisher (events interface): " + socketSimAddressPub,
                                    LOG_LEVEL::INFO);
    }

    void SimComHandler::disconnect() {
        socketSimSub_.disconnect(socketSimAddressSub);
        socketSimSubConfig_.disconnect(socketSimAddressReceiverConfig);
        InterfaceLogger::logMessage("Disconnecting to subscriber (simulation data): " + socketSimAddressSub,
                                    LOG_LEVEL::INFO);
        InterfaceLogger::logMessage("Disconnecting to subscriber (config): " + socketSimAddressReceiverConfig,
                                    LOG_LEVEL::INFO);

    }

    void SimComHandler::close() {
        socketSimSub_.close();
        socketSimPub_.close();
        socketSimSubConfig_.close();
    }

    void SimComHandler::run() {
        simComHandlerThread = std::thread(&sim_interface::SimComHandler::receive, this);

    }

    SimComHandler::~SimComHandler() {
        stopThread = false;
        simComHandlerThread.join();
        unbind();
        disconnect();
        close();
    }




}
