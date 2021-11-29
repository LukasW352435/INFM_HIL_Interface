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
 * // TODO add all authors
 * @version 1.0
 */

#include "SimComHandler.h"

#include <utility>
#include <zmq.hpp>
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
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <boost/algorithm/string.hpp>




#include <thread>

#include <boost/archive/xml_oarchive.hpp>

#include "../DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"

namespace pt = boost::property_tree;
using namespace std;
using boost::property_tree::ptree;
namespace sim_interface {
    SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface,
                                 const std::string& socketSimAddressSub, zmq::context_t &context_sub,
                                 const std::string& socketSimAddressPub, zmq::context_t &context_pub,
                                 const std::string& socketSimAddressSubConfig, zmq::context_t &context_subConfig)
            : queueSimToInterface(std::move(queueSimToInterface)), socketSimSub_(context_sub, zmq::socket_type::sub) , socketSimPub_(context_pub, zmq::socket_type::pub), socketSimSubConfig_(context_subConfig,zmq::socket_type::sub)  {

        socketSimSub_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        socketSimSubConfig_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
        // Connect to publisher
        std::cout << "Connecting to " << socketSimAddressSub << " . . ." << std::endl;
        //  socket_sub.connect(socketSimAdress);

        // Open the connection
        std::cout << "Binding to " << socketSimAddressPub << " . . ." << std::endl;
        socketSimPub_.bind(socketSimAddressPub);




        socketSimSub_.connect(socketSimAddressSub);
        socketSimSubConfig_.connect(socketSimAddressSubConfig);


    }
    SimComHandler::connectorType SimComHandler::resolveConnectorTypeForSwitch(std::string connectorTypeS) {
        if(connectorTypeS == "RESTDummyConnector") return RESTDummyConnector;
        if(connectorTypeS == "CANConnector") return CANConnector;
        return Invalid_Connector;

    }
    void SimComHandler::getConfig()
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
      // std::cout << "CHAR [" << buf << "]" << std::endl;
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
              cout << "HALLOOOOOO" <<connectorTypeS << endl;
              switch (resolveConnectorTypeForSwitch(connectorTypeS)) {
                  case RESTDummyConnector: {

                        std::ostringstream oss;
                        auto optionszzz = pt::xml_writer_make_settings<std::string>(' ', 4);

                 //  v.second.push_front({"boost_serialization signature=\"serialization::archive\" version=\"16\"", decltype(v.second)()});
                 //    v.second.push_front({"!DOCTYPE boost_serialization", decltype(v.second)()});
                     std::stringstream test;
                      boost::property_tree::xml_parser::write_xml(test,v.second,optionszzz);
                      //  v.second.insert(v.second.begin(), {test, decltype(v.second)()});
       // v.second.add("test", v.second);

                 //  cout << "XML STRING " << test.str() << endl;



                    std::string filename = "XMLTESTGenerated.xml";
                   //     std::remove(filename.c_str());
                      std::ifstream in(filename);
              std::stringstream s ;
              boost::property_tree::xml_parser::write_xml(s,v.second,optionszzz);
              std::string s1 = R"(<?xml version="1.0" encoding="utf-8"?>)";
              std::string s2 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE boost_serialization>
<boost_serialization signature="serialization::archive" version="16">
)";
              std::string s3 = s.str();
              s3.replace(s.str().find(s1),s1.length(),s2);
//  boost::trim(s3);
std::istringstream t(s3);
cout << "XML STRING " << s3 << endl;
                      boost::archive::xml_iarchive xmlInputArchive(in);
                     // dut_connector::rest_dummy::RESTConnectorConfig restConnectorConfig =     dut_connector::rest_dummy::RESTConnectorConfig("baseUURL//", "baseCallback//", 222, {"Operation"},{{"Operation",1}} );
                   //   xmlInputArchive >> BOOST_SERIALIZATION_NVP(restConnectorConfig);
                    dut_connector::rest_dummy::RESTConnectorConfig *restConnectorConfig = nullptr;
                      xmlInputArchive >> boost::serialization::make_nvp("restConnectorConfig", restConnectorConfig);
                      in.close();

//                         std::ostringstream xz;
//   xz << in.rdbuf();
//    std::string xs = t.str();
//    std::istringstream ssx(xs);


/*
     std::stringstream s ;
     boost::property_tree::xml_parser::write_xml(s,v.second,optionszzz);
     std::string s1 = R"(<?xml version="1.0" encoding="utf-8"?>)";
                      std::string s2 = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE boost_serialization>
<boost_serialization signature="serialization::archive" version="16">
)";
                      std::string s3 = s.str();
                     s3.replace(s.str().find(s1),s1.length(),s2);
                    //  boost::trim(s3);
                    std::istringstream t(s3);
                      cout << "XML STRING " << s3 << endl;
 //boost::archive::xml_iarchive xmlInputArchive(t);

                      boost::archive::xml_iarchive xmlInputArchive(in);
                     // boost::archive::text_iarchive testt(stringStream);

              //  xmlInputArchive & BOOST_SERIALIZATION_NVP( v.second);

                   // boost::archive::xml_iarchive xmlInputArchive(test);
               //     boost::property_tree::serialize( xmlInputArchive,v.second,  1);

                     //TODO working emty REST constructor
                    //  dut_connector::rest_dummy::RESTConnectorConfig * restConnectorConfig;
              //       std::unique_ptr< dut_connector::rest_dummy::RESTConnectorConfig>  restConnectorConfig;
                  //    dut_connector::rest_dummy::RESTConnectorConfig restConnectorConfig;
                      dut_connector::rest_dummy::RESTConnectorConfig restConnectorConfig =     dut_connector::rest_dummy::RESTConnectorConfig("baseUURL//", "baseCallback//", 222, {"Operation"},{{"Operation",1}} );
                       //sim_interface::dut_connector::ConnectorConfig ccc = sim_interface::dut_connector::ConnectorConfig({"Penis","Speed"},{{"Penis",1}});
                      std::cout << "HÄÄÄ" << std::endl;
                   xmlInputArchive & BOOST_SERIALIZATION_NVP(restConnectorConfig);
                     in.close();

*/

    //     std::ofstream ofss("XMLTESTGenerated.xml");
    //      boost::archive::xml_oarchive xmlArchivee(ofss);

    //     xmlArchivee & BOOST_SERIALIZATION_NVP(restConnectorConfig);


    //     ofss.close();


                      //xmlArchive << cc;


                      break;
                  }
                  case CANConnector: {

                      break;
                  }
                  default: {

                      break;
                  }

          //    }

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
