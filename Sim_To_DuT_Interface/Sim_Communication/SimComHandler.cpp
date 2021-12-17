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

//#include <utility>
//#include <zmq.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/serialization/map.hpp>
//#include <boost/serialization/variant.hpp>


namespace sim_interface {

    namespace pt = boost::property_tree;
    using boost::property_tree::ptree;


    zmq::context_t context_sub(1);
    SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, const SystemConfig& config)
            : queueSimToInterface(std::move(queueSimToInterface)),  socketSimPub_(context_sub,zmq::socket_type::pub), socketSimSub_(context_sub,zmq::socket_type::sub),socketSimSubConfig_(context_sub,zmq::socket_type::sub){

        // zmq Subscriber
        std::string socketSimAddressSub = config.socketSimAddressSub;


        // zmq Publisher
        std::string socketSimAddressPub = config.socketSimAddressPub;



        // zmq Reciver Config
       std::string socketSimAddressReciverConfig = config.socketSimAddressReciverConfig;
        zmq::context_t context_recConfig(1);



        // Config Sockets
        socketSimSub_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        socketSimSub_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        socketSimSubConfig_.setsockopt(ZMQ_SUBSCRIBE, "", 0);



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
        if(connectorTypeS == "RESTDummyConnector") return RESTDummyConnector;
        if(connectorTypeS == "CANConnector") return CANConnector;
        if(connectorTypeS == "V2XConnector") return V2XConnector;
        return Invalid_Connector;

    }
 //   void SimComHandler::getConfig(sim_interface::SimToDuTInterface &interface)
    void SimComHandler::getConfig(std::vector<sim_interface::dut_connector::rest_dummy::RESTConnectorConfig*> *RESTConnectorVektor, std::vector<sim_interface::dut_connector::can::CANConnectorConfig*> *CanConnectorVektor)
    {
        zmq::message_t reply;
        try {
            std::cout << "Receiving...Config " << std::endl;
            socketSimSubConfig_.recv(&reply);
        } catch (zmq::error_t cantReceive) {
            std::cerr << "Socket can't receive: " << cantReceive.what() << std::endl;
            // TODO unbind
        }




        const char *buf = static_cast<const char*>(reply.data());
       //  std::cout << "CHAR [" << buf << "]" << std::endl;
        pt::ptree tree;
        //  pt::basic_ptree

        std::ostringstream t;
        t << buf;
        std::string s = t.str();
        std::stringstream stringStream(s);
        //  boost::archive::xml_iarchive xmlInputArchive(stringStream);

        // Parse the XML into the property tree.
        pt::read_xml(stringStream, tree);
        std::string connectorTypeS;
        for (pt::ptree::value_type &v: tree.get_child("connectors")) {
              connectorTypeS =v.second.get<std::string>("<xmlattr>.classType");


              switch (resolveConnectorTypeForSwitch(connectorTypeS)) {
                  case RESTDummyConnector: {


                        auto xmlWriterSettings = pt::xml_writer_make_settings<std::string>(' ', 4);

                        std::stringstream xmlStringStream;
                        boost::property_tree::xml_parser::write_xml(xmlStringStream,v.second,xmlWriterSettings);



                        std::string xmlString = xmlStringStream.str();
                        boost::algorithm::replace_all(xmlString, R"(<?xml version="1.0" encoding="utf-8"?>)","");



                        sim_interface::dut_connector::rest_dummy::RESTConnectorConfig* restConnectorConfig;
                        std::istringstream xmliStringStream(xmlString);
                        ConfigSerializer::deserialize(xmliStringStream, "conn", &restConnectorConfig);
                        ConfigSerializer::serialize("DasGehtSafeNicht.xml", "conn", restConnectorConfig);

                        // Create the REST connector
                        // Push into vector





                       //connectorConfig->push_back(restConnectorConfig.get());

                      break;
                  }
                  case CANConnector: {


                      auto xmlWriterSettings = pt::xml_writer_make_settings<std::string>(' ', 4);

                      std::stringstream xmlStringStream;
                      boost::property_tree::xml_parser::write_xml(xmlStringStream,v.second,xmlWriterSettings);



                      std::string xmlString = xmlStringStream.str();
                      boost::algorithm::replace_all(xmlString, R"(<?xml version="1.0" encoding="utf-8"?>)","");
                      std::istringstream xmliStringStream(xmlString);

                      sim_interface::dut_connector::can::CANConnectorConfig* canConnectorConfig;

                      ConfigSerializer::deserialize(xmliStringStream, "conn", &canConnectorConfig);
                      ConfigSerializer::serialize("DasGehtSafeNichtCAN.xml", "conn", canConnectorConfig);


                      break;
                  }

                  case V2XConnector: {


                      auto xmlWriterSettings = pt::xml_writer_make_settings<std::string>(' ', 4);

                      std::stringstream xmlStringStream;
                      boost::property_tree::xml_parser::write_xml(xmlStringStream,v.second,xmlWriterSettings);



                      std::string xmlString = xmlStringStream.str();
                      boost::algorithm::replace_all(xmlString, R"(<?xml version="1.0" encoding="utf-8"?>)","");
                      std::istringstream xmliStringStream(xmlString);

                      sim_interface::dut_connector::v2x::V2XConnectorConfig* v2xConnectorConfig;

                      ConfigSerializer::deserialize(xmliStringStream, "conn", &v2xConnectorConfig);
                      ConfigSerializer::serialize("DasGehtSafeNichtV2X.xml", "conn", v2xConnectorConfig);



                      break;
                  }
                  default: {

                      break;
                  }


              }
        }



    }

    void SimComHandler::run() {
        // TODO async receive events from the Simulation and send them to the interface

        while (1) {
            zmq::message_t reply;
            try {
                std::cout << "Receiving... " << std::endl;
                socketSimSub_.recv(&reply);

            } catch (zmq::error_t cantReceive) {
                std::cerr << "Socket can't receive: " << cantReceive.what() << std::endl;
                // TODO unbind
            }

            const char *buf = static_cast<const char*>(reply.data());
            std::cout << "CHAR [" << buf << "]" << std::endl;

            std::string input_data_( buf, reply.size() );
            std::istringstream archive_stream(input_data_);
            boost::archive::text_iarchive archive(archive_stream);
            std::map <std::string, boost::variant<int, double, std::string>> receiveMap , receiveMapDynamics;

            try
            {
                archive >> receiveMap ;
                archive >> receiveMapDynamics;
            } catch (boost::archive::archive_exception& ex) {
                std::cout << "Archive Exception during deserializing:" << std::endl;
                std::cout << ex.what() << std::endl;
            } catch (int e) {
                std::cout << "EXCEPTION " << e << std::endl;
            }

            std::vector<std::string> keyVector;
            std::vector<boost::variant<int, double, std::string>> valueVector;
            for (auto const& element: receiveMap) {
                keyVector.push_back(element.first);
                valueVector.push_back(element.second);
                std::string keyAsString = element.first;

                auto valueAsAny =   element.second;
                std::stringstream stringStreamValue ;
                stringStreamValue <<  valueAsAny;
                //TODO "Simulation Traci" würde man auch aus dem Archive bekommen vllt anpassen
                SimEvent event(keyAsString, stringStreamValue.str(), "Simulation Traci");
                sendEventToInterface(event);
            }
            for (auto const& element: receiveMapDynamics) {
                keyVector.push_back(element.first);
                valueVector.push_back(element.second);
                std::string keyAsString = element.first;

                auto valueAsAny =   element.second;
                std::stringstream stringStreamValue ;
                stringStreamValue <<  valueAsAny;
                // std::cout << "KEY: " << keyAsString << std::endl;
                //std::cout << "value: " << stringStreamValue.str() << std::endl;
                //TODO "Simulation Traci" würde man auch aus dem Archive bekommen vllt anpassen
                SimEvent event(keyAsString, stringStreamValue.str(), "Simulation Dynamics");
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
        std::map<std::string , boost::variant<int, double, std::string>> simEventMap;
        simEventMap["Operation"] = simEvent.operation;
        simEventMap["Value"]     = boost::apply_visitor(EventVisitor(), simEvent.value);
        simEventMap["Origin"]    = simEvent.origin;
        simEventMap["Current"]   = simEvent.current;
        //serialize map
        std::ostringstream ss;
        boost::archive::text_oarchive archive(ss);
        archive << simEventMap;
        std::string outbound_data = ss.str();
        // create buffer size for message
        zmq::message_t msgToSend(outbound_data);

        socketSimPub_.send(msgToSend);
    }

    void SimComHandler::sendEventToInterface(const SimEvent &simEvent) {
        InterfaceLogger::logEvent(simEvent);
        queueSimToInterface->push(simEvent);
    }

    SimComHandler::~SimComHandler() {
        // TODO end zmq, etc.
    }


}
