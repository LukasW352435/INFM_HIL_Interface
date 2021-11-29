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
 * @version 1.0
 */

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
    /**
     * Handler between interface and simulation.
     * Responsible for sending/receiving SimEvents to/from the simulation.
     */
    class SimComHandler {
    public:
        /**
         * Create a new handler.
         * @param queueSimToInterface A queue to communicate with the interface.
         * @param socketSimAddressSub
         * @param context_sub
         * @param socketSimAddressPub
         * @param context_pub
         */
        SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, const std::string& socketSimAddressSub,
                      zmq::context_t &context_sub , const std::string& socketSimAddressPub,
                      zmq::context_t &context_pub,
                      const std::string& socketSimAddressSubConfig, zmq::context_t &context_subConfig);

        /**
         * Destroys the handler and stops all threads.
         */
        ~SimComHandler();

        /**
         * Asynchronous sending of events to simulation.
         * @param simEvent An event that should be sent to the simulation.
         */
        void sendEventToSim(const SimEvent &simEvent);

        /**
         * Starts the handler to asynchronously receive incoming events.
         */
        void run();
        void getConfig();
        enum connectorType {
            RESTDummyConnector,
            CANConnector,
            Invalid_Connector
        };
        connectorType resolveConnectorTypeForSwitch(std::string connectorTypeS);



    private:
        // send an event to the interface
        void sendEventToInterface(const SimEvent &simEvent);

        zmq::socket_t socketSimSub_;
        zmq::socket_t socketSimPub_;
        zmq::context_t context_test;
        zmq::socket_t socketSimSubConfig_;

        // used by sendEventToInterface
        std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface;
    };
}

#endif //SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
