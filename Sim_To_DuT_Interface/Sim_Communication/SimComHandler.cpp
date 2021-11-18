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

namespace sim_interface {
    SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface,
                                 std::string socketSimAddress, zmq::context_t &context_sub)
            : queueSimToInterface(std::move(queueSimToInterface)), socketSim_(context_sub, zmq::socket_type::sub) {
        //create a subscriber socket
        // zmq::context_t context_sub(1);
        //  zmq::socket_type type_sub = zmq::socket_type::sub;
        //  zmq::socket_t socket_sub(context_sub,type_sub);
        // socket_sub.setsockopt(ZMQ_SUBSCRIBE, "" ,0);
        socketSim_.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        // Connect to publisher
        std::cout << "Connecting to " << socketSimAddress << " . . ." << std::endl;
        //  socket_sub.connect(socketSimAdress);
        socketSim_.connect(socketSimAddress);
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

    std::string SimComHandler::getMessageFromSim() {

        // create a subscriber socket
        // zmq::context_t context_sub(1);
        // zmq::socket_type type_sub = zmq::socket_type::sub;
        //  zmq::socket_t socket_sub(context_sub,type_sub);
        // socket_sub.setsockopt(ZMQ_SUBSCRIBE, "" ,0);


        // Connect to publisher
        // cout << "Connecting to " << PUBLISH_ENDPOINT << " . . ." << endl;
        // socket_sub.connect(PUBLISH_ENDPOINT);


        //Pause to connect
        //  this_thread::sleep_for(chrono::milliseconds(1000));

        zmq::message_t message_sub;

        socketSim_.recv(message_sub, zmq::recv_flags::none);
        std::cout << "Empfangene Nachricht: " << message_sub.to_string() << std::endl;
        return message_sub.to_string();
    }

    SimComHandler::~SimComHandler() {
        // TODO end all Communication, Close all Handles, End all Async operations, join all Threads
    }
}
