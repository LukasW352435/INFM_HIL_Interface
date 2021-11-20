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
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>
namespace sim_interface {
    SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface,
                                 const std::string& socketSimAddressSub, zmq::context_t &context_sub,
                                 const std::string& socketSimAddressPub, zmq::context_t &context_pub)
            : queueSimToInterface(std::move(queueSimToInterface)), socketSimSub_(context_sub, zmq::socket_type::sub) , socketSimPub_(context_pub, zmq::socket_type::pub)  {

        socketSimSub_.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        // Connect to publisher
        std::cout << "Connecting to " << socketSimAddressSub << " . . ." << std::endl;
        //  socket_sub.connect(socketSimAdress);

        // Open the connection
        std::cout << "Binding to " << socketSimAddressPub << " . . ." << std::endl;
        socketSimPub_.bind(socketSimAddressPub);





        socketSimSub_.connect(socketSimAddressSub);

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
            std::map <std::string, boost::variant<int, double, std::string>> receiveMap;

            try
            {
                archive >> receiveMap;
            } catch (boost::archive::archive_exception& ex) {
                std::cout << "Archive Exception during deserializing:" << std::endl;
                std::cout << ex.what() << std::endl;
            } catch (int e) {
                std::cout << "EXCEPTION " << e << std::endl;
            }
            std::cout << "Value " << receiveMap["Speed"]<< std::endl;
            //  std::string test  = receiveMap["Speed"].which();
            std::vector<std::string> keyVector;
            std::vector<boost::variant<int, double, std::string>> valueVector;
            for (auto const& element: receiveMap) {
                keyVector.push_back(element.first);
                valueVector.push_back(element.second);
                std::string keyAsString = element.first;

                auto valueAsAny =   element.second;
                std::stringstream stringStreamValue ;
                stringStreamValue <<  valueAsAny;

                std::cout << "value: " << stringStreamValue.str() << std::endl;
                SimEvent event(keyAsString, stringStreamValue.str(), "Simulation");
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




}
