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
 * // TODO add all authors
 * @version 1.0
 */

#include "SimComHandler.h"

#include <utility>
#include <zmq.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>

#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>

#include "../DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"
#include "../DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "../Utility/ConfigSerializer.h"


namespace sim_interface {

    namespace pt = boost::property_tree;

using boost::property_tree::ptree;
zmq::context_t context_subb(1);
    SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, const SystemConfig& config)
            : queueSimToInterface(std::move(queueSimToInterface)), socketSimPub_(context_subb,zmq::socket_type::pub), socketSimSub_(context_subb,zmq::socket_type::sub),socketSimSubConfig_(context_subb,zmq::socket_type::sub) {


        // zmq Subscriber
        std::string socketSimAddressSub = config.socketSimAddressSub;
        zmq::context_t context_sub(1);

        // zmq Publisher
        std::string socketSimAddressPub = config.socketSimAddressPub;
        zmq::context_t context_pub(1);


        // zmq Reciver Config
       std::string socketSimAddressReciverConfig = config.socketSimAddressReciverConfig;
      zmq::context_t context_recConfig(1);



        // Config Sockets
         socketSimSub_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
         socketSimSubConfig_.setsockopt(ZMQ_SUBSCRIBE, "", 0);



        // Connect to publisher
       std::cout << "Connecting to " << socketSimAddressSub << " . . ." << std::endl;
       std::cout << "Connecting to " << socketSimAddressReciverConfig << " . . ." << std::endl;
       //  socket_sub.connect(socketSimAdress);

       // Open the connection
       std::cout << "Binding to " << socketSimAddressPub << " . . ." << std::endl;
       socketSimPub_.bind(socketSimAddressPub);
       socketSimSub_.connect(socketSimAddressSub);
       socketSimSubConfig_.connect(socketSimAddressReciverConfig);


    }
    SimComHandler::connectorType SimComHandler::resolveConnectorTypeForSwitch(std::string connectorTypeS) {
        if(connectorTypeS == "RESTDummyConnector") return RESTDummyConnector;
        if(connectorTypeS == "CANConnector") return CANConnector;
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
    //     std::cout << "CHAR [" << buf << "]" << std::endl;
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


                        auto optionszzz = pt::xml_writer_make_settings<std::string>(' ', 4);

                        std::stringstream test;
                        boost::property_tree::xml_parser::write_xml(test,v.second,optionszzz);


                        std::stringstream s ;
                        pt::ptree testBaum;

                        //XML String in richtige Form bringen
                        testBaum.add_child("scoped_ptr",v.second);
                        boost::property_tree::xml_parser::write_xml(s,testBaum,optionszzz);
                        std::string s1 = R"(<?xml version="1.0" encoding="utf-8"?>)";
                        std::string s3 = s.str();
                        s3.replace(s.str().find(s1),s1.length(),"");
                        std::string s4 = R"(classType=")"  +connectorTypeS + R"(")";
                        s3.replace(s3.find(s4), s4.length(),"");




                        std::istringstream xmlStringStream(s3);

                        std::string fileName = connectorTypeS + ".xml";
                        std::ofstream RESTConnectorConfigXMLFile(fileName);
                        RESTConnectorConfigXMLFile << xmlStringStream.rdbuf();

                        // TODO Make smart try catch
                        boost::scoped_ptr<sim_interface::dut_connector::rest_dummy::RESTConnectorConfig> restConnectorConfig;

                        ConfigSerializer::deserialize(fileName, "conn", restConnectorConfig);
                        //  ConfigSerializer::serialize("DasGehtSafeNicht.xml", "conn", restConnectorConfig);

                        // Create the REST connector
                        // Push into vector
                        RESTConnectorVektor->push_back(  new sim_interface::dut_connector::rest_dummy::RESTConnectorConfig ("http://localhost:9090",
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
                                                                                                                   "origin",
                                                                                                                   "SpeedDynamics",
                                                                                                                   "YawRateDynamics",
                                                                                                                   "AccelerationDynamics",
                                                                                                                   "HeadingDynamics",
                                                                                                                                                                                                                                                                                                                                                                                                                                                                     "LatitudeDynamics",
                                                                                                                   "LongitudeDynamics",
                                                                                                                   "PosXDynamics",
                                                                                                                   "PoYDynamics"},
                                                                                                                  {{"Test", 1000}},
                                                                                                                  true));




                       //connectorConfig->push_back(restConnectorConfig.get());

                      break;
                  }
                  case CANConnector: {

                        auto optionszzz = pt::xml_writer_make_settings<std::string>(' ', 4);

                        std::stringstream test;
                        boost::property_tree::xml_parser::write_xml(test,v.second,optionszzz);


                        std::stringstream s ;
                        pt::ptree testBaum;

                      //XML String in richtige Form bringen
                        testBaum.add_child("scoped_ptr",v.second);
                        boost::property_tree::xml_parser::write_xml(s,testBaum,optionszzz);
                        std::string s1 = R"(<?xml version="1.0" encoding="utf-8"?>)";
                        std::string s3 = s.str();
                        s3.replace(s.str().find(s1),s1.length(),"");
                        std::string s4 = R"(classType=")"  +connectorTypeS + R"(")";
                        s3.replace(s3.find(s4), s4.length(),"");




                        std::istringstream xmlStringStreamCanConnector(s3);




                      std::string fileNameCan = connectorTypeS + ".xml";
                      std::ofstream canConnectorConfigXMLFile(fileNameCan);
                      canConnectorConfigXMLFile << xmlStringStreamCanConnector.rdbuf();

                        //   std::cout << "WTF " << s3 << std::endl;
                        //   boost::scoped_ptr<sim_interface::dut_connector::can::CANConnectorConfig> canConnectorConfig;
                        //   ConfigSerializer::deserialize(fileNameCan, "conn", canConnectorConfig);
                        //   ConfigSerializer::serialize("HalloCanVonSim.xml", "conn", canConnectorConfig);
                        //   connectorConfig->push_back(canConnectorConfig.get());

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
                              0x222,
                              false,
                              false
                      );

                      // CANFD non cyclic send operation
                      sim_interface::dut_connector::can::CANConnectorSendOperation sendOpCanfd1{
                              0x333,
                              true,
                              false,
                      };

                      // CANFD cyclic send operation
                      struct bcm_timeval ival1 = {0};
                      ival1.tv_sec = 1;
                      ival1.tv_usec = 0;

                      struct bcm_timeval ival2 = {0};
                      ival2.tv_sec = 3;
                      ival2.tv_usec = 0;

                      sim_interface::dut_connector::can::CANConnectorSendOperation sendOpCyclicCanfd1(
                              0x444,
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
                              {"Door", sendOpCanfd1},
                              {"Blink", sendOpCyclicCanfd1}
                      };




                      boost::scoped_ptr<sim_interface::dut_connector::can::CANConnectorConfig> canConfigTest ( new sim_interface::dut_connector::can::CANConnectorConfig(
                              "vcan0",
                              "BmwCodec",
                              {"Speed", "Door", "Blink", "Hazard", "Brake"},
                              frameToOperation,
                              operationToFrame,
                              {},
                              false));


                      CanConnectorVektor->push_back( new  sim_interface::dut_connector::can::CANConnectorConfig (
                              "vcan0",
                              "BmwCodec",
                              {"Speed", "Door", "Blink", "Hazard", "Brake"},
                              frameToOperation,
                              operationToFrame,
                              {},
                              false));



                      break;
                  }

                  case V2XConnector: {

                      auto optionszzz = pt::xml_writer_make_settings<std::string>(' ', 4);

                      std::stringstream test;
                      boost::property_tree::xml_parser::write_xml(test,v.second,optionszzz);


                      std::stringstream s ;
                      pt::ptree testBaum;

                      //XML String in richtige Form bringen
                      testBaum.add_child("scoped_ptr",v.second);
                      boost::property_tree::xml_parser::write_xml(s,testBaum,optionszzz);
                      std::string s1 = R"(<?xml version="1.0" encoding="utf-8"?>)";
                      std::string s3 = s.str();
                      s3.replace(s.str().find(s1),s1.length(),"");
                      std::string s4 = R"(classType=")"  +connectorTypeS + R"(")";
                      s3.replace(s3.find(s4), s4.length(),"");




                      std::istringstream xmlStringStreamV2XConnector(s3);




                      std::string fileNameV2X = connectorTypeS + ".xml";
                      std::ofstream V2XConnectorConfigXMLFile(fileNameV2X);
                      V2XConnectorConfigXMLFile << xmlStringStreamV2XConnector.rdbuf();

                      //   std::cout << "WTF " << s3 << std::endl;
                      //   boost::scoped_ptr<sim_interface::dut_connector::can::CANConnectorConfig> canConnectorConfig;
                      //   ConfigSerializer::deserialize(fileNameCan, "conn", canConnectorConfig);
                      //   ConfigSerializer::serialize("HalloCanVonSim.xml", "conn", canConnectorConfig);
                      //   connectorConfig->push_back(canConnectorConfig.get());




                      break;
                  }
                  default: {

                      break;
                  }


              }
        }
        //    std::string input_data_( buf, reply.size() );
        //    std::istringstream archive_stream(input_data_);
        //    boost::archive::xml_iarchive archive(archive_stream);
        //    //std::map <std::string, std::string> receiveMap;
        //   std::map<std::string,std::multimap<std::string,std::string>> receiveMap;

        //   try
        //   {
        //       archive >> receiveMap;
        //   } catch (boost::archive::archive_exception& ex) {
        //       std::cout << "Archive Exception during deserializing:" << std::endl;
        //       std::cout << ex.what() << std::endl;
        //   } catch (int e) {
        //       std::cout << "EXCEPTION " << e << std::endl;
        //   }

        //   //  std::string test  = receiveMap["Speed"].which();
        //   std::vector<std::string> keyVector;
        //   std::vector<std::string> valueVector;
        //   std::multimap<std::string,std::string> testCanMap;
        //   testCanMap  = receiveMap.at("CANConnector");


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
        simEventMap["Value"]     = simEvent.value;
        simEventMap["Origin"]    = simEvent.origin;
        std::stringstream time;
        time << simEvent.current;
        // Time in Secondss
        // TODO Microsekunden
        simEventMap["Currrent"]   = time.str();
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
        queueSimToInterface->push(simEvent);
    }

    SimComHandler::~SimComHandler() {
        // TODO end zmq, etc.
    }


}
