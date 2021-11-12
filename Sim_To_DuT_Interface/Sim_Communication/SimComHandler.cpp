//
// Created by Lukas on 19.10.2021.
//

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
zmq::context_t context_pubTest(1);
namespace sim_interface {
    SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface,
                                 const std::string& socketSimAddressSub, zmq::context_t &context_sub,
                                 const std::string& socketSimAddressPub, zmq::context_t &context_pub)
            : queueSimToInterface(std::move(queueSimToInterface)), socketSimSub_(context_sub, zmq::socket_type::sub) , socketSimPub_(context_pubTest, zmq::socket_type::pub)  {
        //create a subscriber socket
        // zmq::context_t context_sub(1);
        //  zmq::socket_type type_sub = zmq::socket_type::sub;
        //  zmq::socket_t socket_sub(context_sub,type_sub);
        // socket_sub.setsockopt(ZMQ_SUBSCRIBE, "" ,0);
        socketSimSub_.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        // Connect to publisher
        std::cout << "Connecting to " << socketSimAddressSub << " . . ." << std::endl;
        //  socket_sub.connect(socketSimAdress);

        // Open the connection
        std::cout << "Binding to " << socketSimAddressPub << " . . ." << std::endl;
        socketSimPub_.bind(socketSimAddressPub);





        socketSimSub_.connect(socketSimAddressSub);
        // socketSimSub_ = socket_sub;
        //  std::string t =  SimComHandler::getMessageFromSim(socketSimSub_);

        //  std::cout << "Wuhhh " + t <<  std::endl;
    }


    void SimComHandler::run() {
        // TODO async receive events from the Simulation and send them to the interface


        zmq::message_t reply;
        socketSimSub_.recv(&reply);

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
        std::vector<std::string> keyVektor;
        std::vector<boost::variant<int, double, std::string>> valueVektor;
        for (auto const& element: receiveMap) {
            keyVektor.push_back(element.first);
            valueVektor.push_back(element.second);
            std::string keyAsString = element.first;

            auto valueAsAny =   element.second;
            std::stringstream stringStreamValue ;
            stringStreamValue <<  valueAsAny;

            std::cout << "value: " << stringStreamValue.str() << std::endl;
            SimEvent event(keyAsString, stringStreamValue.str(), "Simulation");
        }




        sendEventToInterface(event);
    }

    void SimComHandler::sendEventToSim(const SimEvent &simEvent) {
        //BOYS hier müssen wir hin
        // TODO implementation of sending an event to the simulation
        std::cout << "Async Sending of Event..." << std::endl;
        std::cout << simEvent << "lol";
            // Send it off to any subscribers
        std::cout << "Waiting to Send " << std::endl;

        socketSimPub_.send(zmq::buffer("Value: " + simEvent.value + " Operation: " + simEvent.operation));
    }

    void SimComHandler::sendEventToInterface(const SimEvent &simEvent) {
        queueSimToInterface->push(simEvent);
    }




}
