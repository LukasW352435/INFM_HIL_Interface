//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
#define SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H

#include <memory>
#include "../Events/SimEvent.h"
#include <string>
#include "../Utility/SharedQueue.h"
#include <thread>
#include <chrono>
#include <zmq.hpp>

class SimComHandler {
public:
    SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, std::string socketSimAdress, zmq::context_t &context_sub);

    // async send event to simulation
    void sendEventToSim(const SimEvent &simEvent);

    // run async receive incoming events
    void run();
     std::string getMessageFromSim();
private:
    // send an event to the interface
    void sendEventToInterface(const SimEvent &simEvent);
    zmq::socket_t socketSim_;
    zmq::context_t context_test;

    // used by sendEventToInterface
    std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface;
};

#endif //SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
