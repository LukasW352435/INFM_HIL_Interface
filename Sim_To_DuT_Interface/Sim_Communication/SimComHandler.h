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
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
namespace sim_interface {
    class SimComHandler {
    public:
        SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, const std::string& socketSimAddressSub,
                      zmq::context_t &context_sub , const std::string& socketSimAddressPub,
                      zmq::context_t &context_pub);

        // async send event to simulation
        void sendEventToSim(const SimEvent &simEvent);

        // run async receive incoming events
        void run();

        std::string getMessageFromSim();

    private:
        // send an event to the interface
        void sendEventToInterface(const SimEvent &simEvent);

        zmq::socket_t socketSimSub_;
        zmq::socket_t socketSimPub_;
        zmq::context_t context_test;

        // used by sendEventToInterface
        std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface;
    };
}

#endif //SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
