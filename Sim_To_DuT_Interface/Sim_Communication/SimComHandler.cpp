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
   * Global Variable:
   * @param context_sub: Defines the context of the zmq socket
   */
    zmq::context_t context_sub(1);

    SimComHandler::SimComHandler(SimToDuTInterface *interface, const SystemConfig &config)
            : interface(interface), socketSimPub_(context_sub, zmq::socket_type::pub),
              socketSimSub_(context_sub, zmq::socket_type::sub),
              socketSimSubConfig_(context_sub, zmq::socket_type::sub) {

        // zmq Subscriber
        std::string socketSimAddressSub = config.socketSimAddressSub;


        // zmq Publisher
        std::string socketSimAddressPub = config.socketSimAddressPub;



        // zmq Reciver Config
        std::string socketSimAddressReciverConfig = config.socketSimAddressReciverConfig;
        zmq::context_t context_recConfig(1);



        // Config Sockets
        socketSimSub_.set(zmq::sockopt::subscribe, "");
        socketSimSubConfig_.set(zmq::sockopt::subscribe, "");

        // Connect to publisher
        std::cout << "Connecting to " << socketSimAddressSub << " . . ." << std::endl;
        std::cout << "Connecting to " << socketSimAddressReciverConfig << " . . ." << std::endl;


        // Open the connection
        std::cout << "Binding to " << socketSimAddressPub << " . . ." << std::endl;
        socketSimPub_.bind(socketSimAddressPub);
        socketSimSub_.connect(socketSimAddressSub);
        socketSimSubConfig_.connect(socketSimAddressReciverConfig);


    }

    SimComHandler::connectorType SimComHandler::resolveConnectorTypeForSwitch(std::string connectorTypeS) {
        if (connectorTypeS == "RESTDummyConnector") return RESTDummyConnector;
        if (connectorTypeS == "CANConnector") return CANConnector;
        if (connectorTypeS == "V2XConnector") return V2XConnector;
        return Invalid_Connector;

    }


    //   void SimComHandler::getConfig(sim_interface::SimToDuTInterface &interface)
    void SimComHandler::getConfig() {
        zmq::message_t reply;
        try {
            std::cout << "Receiving...Config " << std::endl;
            socketSimSubConfig_.recv(reply, zmq::recv_flags::none);
        } catch (zmq::error_t &cantReceive) {
            std::cerr << "Socket can't receive: " << cantReceive.what() << std::endl;
            // TODO unbind
        }


        const char *buf = static_cast<const char *>(reply.data());

        boost::property_tree::ptree tree;


        std::ostringstream t;
        t << buf;
        std::string s = t.str();
        std::stringstream stringStream(s);

        // Parse the XML into the property tree.

        try {
            std::cout << "Receiving...Config " << std::endl;
            boost::property_tree::read_xml(stringStream, tree);
        } catch (std::exception &e) {
            std::cerr << "XML in wrong format " << e.what() << std::endl;
            // TODO (unbind) start Again (and break)
        }
        std::string connectorTypeS;
        auto xmlWriterSettings = boost::property_tree::xml_writer_make_settings<std::string>(' ', 4);
        for (boost::property_tree::ptree::value_type &connector: tree.get_child("connectors")) {
            connectorTypeS = connector.second.get<std::string>("<xmlattr>.classType");


            switch (resolveConnectorTypeForSwitch(connectorTypeS)) {
                case RESTDummyConnector: {

                    std::stringstream xmlStringStream, testStringStream;

                    boost::property_tree::xml_parser::write_xml(xmlStringStream, connector.second, xmlWriterSettings);



                    std::string xmlString = xmlStringStream.str();


                    boost::algorithm::replace_all(xmlString, R"(<?xml version="1.0" encoding="utf-8"?>)", "");


                    sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *restConnectorConfig;
                    std::istringstream xmliStringStream(xmlString);

                    ConfigSerializer::deserialize(xmliStringStream, "conn", &restConnectorConfig);


                    // Create the REST connector
                    // Push into vector


                    auto restConnector = std::make_shared<dut_connector::rest_dummy::RESTDummyConnector>(
                            interface->getQueueDuTToSim(), *restConnectorConfig);
                    interface->addConnector(restConnector);


                    break;
                }
                case CANConnector: {


                    std::stringstream xmlStringStream;
                    boost::property_tree::xml_parser::write_xml(xmlStringStream, connector.second, xmlWriterSettings);


                    std::string xmlString = xmlStringStream.str();

                    boost::algorithm::replace_all(xmlString, R"(<?xml version="1.0" encoding="utf-8"?>)", "");


                    //Workaround because frameToOperation & OperationToframe need class_id_reference to work


                    boost::algorithm::replace_all(xmlString, R"(<frameToOperation>)",
                                                  R"(<frameToOperation class_id="2">)");
                    boost::algorithm::replace_all(xmlString, R"(<operationToFrame>)",
                                                  R"(<operationToFrame class_id="5">)");
                    boost::algorithm::replace_all(xmlString, R"(<itemOperationToFrame>)",
                                                  R"(<itemOperationToFrame class_id_reference="6">)");

                    boost::algorithm::replace_first(xmlString, R"(<itemOperationToFrame class_id_reference="6">)",
                                                    R"(<itemOperationToFrame class_id="6">)");

                    boost::algorithm::replace_all(xmlString, R"(<itemFrameToOperation>)",
                                                  R"(<itemFrameToOperation class_id_reference="3">)");

                    boost::algorithm::replace_first(xmlString, R"(<itemFrameToOperation class_id_reference=3">)",
                                                    R"(<itemFrameToOperation class_id="3">)");
                    boost::algorithm::replace_first(xmlString, R"(<ival1>)",
                                                    R"(<ival1 class_id="8" tracking_level="0">)");
                    boost::algorithm::replace_all(xmlString, R"(<CANConnectorReceiveOperation>)",
                                                  R"(<CANConnectorReceiveOperation class_id_reference="4">)");
                    boost::algorithm::replace_first(xmlString,
                                                    R"(<CANConnectorReceiveOperation class_id_reference="6">)",
                                                    R"(<CANConnectorReceiveOperation class_id="4">)");

                    boost::algorithm::replace_all(xmlString, R"(<CANConnectorSendOperation>)",
                                                  R"(<CANConnectorSendOperation class_id_reference="7">)");
                    boost::algorithm::replace_first(xmlString, R"(<CANConnectorSendOperation class_id_reference="7">)",
                                                    R"(<CANConnectorSendOperation class_id="7">)");

                    std::istringstream xmliStringStream(xmlString);

                    sim_interface::dut_connector::can::CANConnectorConfig *canConnectorConfig;
                    ConfigSerializer::deserialize(xmliStringStream, "conn", &canConnectorConfig);


                    auto CANConnector = std::make_shared<dut_connector::can::CANConnector>(
                            interface->getQueueDuTToSim(), *canConnectorConfig);
                    interface->addConnector(CANConnector);
                    break;
                }

                case V2XConnector: {


                    std::stringstream xmlStringStream;
                    boost::property_tree::xml_parser::write_xml(xmlStringStream, connector.second, xmlWriterSettings);


                    std::string xmlString = xmlStringStream.str();
                    boost::algorithm::replace_all(xmlString, R"(<?xml version="1.0" encoding="utf-8"?>)", "");

                    //Fix xmlString with class_id
                    boost::algorithm::replace_first(xmlString, R"(<conn>)", R"(<conn class_id="0">)");

                    std::istringstream xmliStringStream(xmlString);

                    sim_interface::dut_connector::v2x::V2XConnectorConfig *V2XConnectorConfig;

                    ConfigSerializer::deserialize(xmliStringStream, "conn", &V2XConnectorConfig);
                    auto V2XConnector = std::make_shared<dut_connector::v2x::V2XConnector>(
                            interface->getQueueDuTToSim(), *V2XConnectorConfig);
                    interface->addConnector(V2XConnector);


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

    }


    void SimComHandler::receive() {
        // TODO async receive events from the Simulation and send them to the interface

        while (stopThread) {
            zmq::message_t reply;
            try {
                std::cout << "Receiving... " << std::endl;
                socketSimSub_.recv(reply, zmq::recv_flags::none);

            } catch (zmq::error_t cantReceive) {
                std::cerr << "Socket can't receive: " << cantReceive.what() << std::endl;
                // TODO unbind
            }

            const char *buf = static_cast<const char *>(reply.data());
            std::cout << "CHAR [" << buf << "]" << std::endl;

            std::string input_data_(buf, reply.size());
            std::istringstream archive_stream(input_data_);
            boost::archive::text_iarchive archive(archive_stream);
            std::map<std::string, boost::variant<int, double, std::string>> receiveMap;

            try {
                archive >> receiveMap;
            } catch (boost::archive::archive_exception &ex) {
                std::cout << "Archive Exception during deserializing:" << std::endl;
                std::cout << ex.what() << std::endl;
            } catch (int e) {
                std::cout << "EXCEPTION " << e << std::endl;
            }

            std::vector<std::string> keyVector;
            std::vector<boost::variant<int, double, std::string>> valueVector;
            for (auto const &element: receiveMap) {
                keyVector.push_back(element.first);
                valueVector.push_back(element.second);
                std::string keyAsString = element.first;

                auto valueAsAny = element.second;
                std::stringstream stringStreamValue;
                stringStreamValue << valueAsAny;
                //TODO "Simulation Traci" würde man auch aus dem Archive bekommen vllt anpassen
                SimEvent event(keyAsString, stringStreamValue.str(), "Simulation Traci");
                sendEventToInterface(event);
            }
        }
    }

    void SimComHandler::sendEventToSim(const SimEvent &simEvent) {
        //BOYS hier müssen wir hin
        // TODO implementation of sending an event to the simulation
        std::cout << "Async Sending of Event..." << std::endl;
        std::cout << simEvent << "lol";
        // Send it off to any subscribers
        std::cout << "Waiting to Send " << std::endl;

        // std::map<std::string , boost::variant<int, double, std::string, std::time_t>> simEventMap;
        //Not working with curreent time
        std::map<std::string, boost::variant<int, double, std::string>> simEventMap;
        simEventMap["Operation"] = simEvent.operation;
        simEventMap["Value"] = boost::apply_visitor(EventVisitor(), simEvent.value);
        simEventMap["Origin"] = simEvent.origin;
        simEventMap["Current"] = simEvent.current;
        //serialize map
        std::ostringstream ss;
        boost::archive::text_oarchive archive(ss);
        archive << simEventMap;
        std::string outbound_data = ss.str();
        // create buffer size for message
        zmq::message_t msgToSend(outbound_data);

        socketSimPub_.send(msgToSend, zmq::send_flags::none);
    }

    void SimComHandler::sendEventToInterface(const SimEvent &simEvent) {
        InterfaceLogger::logEvent(simEvent);
        interface->getQueueSimToInterface()->push(simEvent);
    }

    SimComHandler::~SimComHandler() {
        stopThread = false;
        simComHandlerThread.join();
        //socketSimPub_.unbind(config.socketSimAddressPub);
        //socketSimSub_.connect(socketSimAddressSub);
        //socketSimSubConfig_.connect(socketSimAddressReciverConfig);


        // TODO end zmq, etc.
    }

    void SimComHandler::run() {
        simComHandlerThread = std::thread(&sim_interface::SimComHandler::receive, this);

    }


}
