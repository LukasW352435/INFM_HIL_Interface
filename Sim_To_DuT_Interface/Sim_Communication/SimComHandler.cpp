//
// Created by Lukas on 19.10.2021.
//

#include "SimComHandler.h"

#include <utility>
#include <zmq.hpp>

SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, std::string socketSimAdress, zmq::context_t &context_sub)
        : queueSimToInterface(std::move(queueSimToInterface)),  socketSim_(context_sub, zmq::socket_type::sub) {
    std::cout << "Constructor?" << socketSimAdress << std::endl;
    //create a subscriber socket
   // zmq::context_t context_sub(1);
  //  zmq::socket_type type_sub = zmq::socket_type::sub;
  //  zmq::socket_t socket_sub(context_sub,type_sub);
   // socket_sub.setsockopt(ZMQ_SUBSCRIBE, "" ,0);
    socketSim_.setsockopt(ZMQ_SUBSCRIBE, "" ,0);

    // Connect to publisher
    std::cout << "Connecting to " << socketSimAdress << " . . ." << std::endl;
  //  socket_sub.connect(socketSimAdress);
    socketSim_.connect(socketSimAdress);
   // socketSim_ = socket_sub;
  //  std::string t =  SimComHandler::getMessageFromSim(socketSim_);


    //  std::cout << "Wuhhh " + t <<  std::endl;
}

void SimComHandler::run() {
    // TODO async receive events from the Simulation and send them to the interface
    SimEvent event("Test", "Test", "Test");
    sendEventToInterface(event);
}

void SimComHandler::sendEventToSim(const SimEvent &simEvent) {
    // TODO implementation of sending an event to the simulation
    std::cout << "Async Sending of Event..." << std::endl;
    std::cout << simEvent << "lol";
}

void SimComHandler::sendEventToInterface(const SimEvent &simEvent) {
    queueSimToInterface->push(simEvent);
}





 std::string SimComHandler::getMessageFromSim(  ) {
/*
     //create a subscriber socket
     zmq::context_t context_sub(1);
     zmq::socket_type type_sub = zmq::socket_type::sub;
   //  zmq::socket_t socket_sub(context_sub,type_sub);
     socket_sub.setsockopt(ZMQ_SUBSCRIBE, "" ,0);


     // Connect to publisher
     cout << "Connecting to " << PUBLISH_ENDPOINT << " . . ." << endl;
     socket_sub.connect(PUBLISH_ENDPOINT);
*/

     //Pause to connect
   //  this_thread::sleep_for(chrono::milliseconds(1000));

         zmq::message_t message_sub;

     socketSim_.recv( message_sub, zmq::recv_flags::none);
     std::cout << message_sub.to_string() << "!!!" << std::endl;
        return message_sub.to_string();
}
/*
SimComHandler::SimComHandler(std::string socketSimAdress) {
    std::cout << "Constructor?" << socketSimAdress << std::endl;
    //create a subscriber socket
    zmq::context_t context_sub(1);
    zmq::socket_type type_sub = zmq::socket_type::sub;
    zmq::socket_t socket_sub(context_sub,type_sub);
    socket_sub.setsockopt(ZMQ_SUBSCRIBE, "" ,0);


    // Connect to publisher
    std::cout << "Connecting to " << socketSimAdress << " . . ." << std::endl;
    socket_sub.connect(socketSimAdress);
    socketSim_ = &socket_sub;
   // std::string t =  SimComHandler::getMessageFromSim(socketSim_);


  //  std::cout << "Wuhhh " + t <<  std::endl;
}
*/



