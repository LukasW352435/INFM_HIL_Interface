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
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H

#include <string>
#include <iostream>
#include <set>
#include <memory>
#include <boost/asio.hpp>
#include "../DuT_Connectors/ConnectorInfo.h"
#include "../Events/SimEvent.h"
#include "../Utility/SharedQueue.h"
#include "../Utility/PeriodicTimer.h"
#include "ConnectorConfig.h"

namespace sim_interface::dut_connector {
    /// Connector implementing all kinds of DuT devices.
    /// The different connectors should implement the protocol to communicate with a specific hardware DuT.
    /// Used in the interface to send/receive events to/from the device.
    class DuTConnector {
    public:
        /// Create a new connector.
        /// \param queueDuTToSim Queue to communicate with the interface.
        /// \param config Config for the device.
        explicit DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                              const sim_interface::dut_connector::ConnectorConfig &config);

        /// Destroy the connector, stop all operations, end all threads.
        ~DuTConnector();

        /// Some basic information from the connector.
        /// Please override this methode!
        /// \return Some information and version of the device.
        virtual ConnectorInfo getConnectorInfo();

        /// Handles an event asynchronously from the simulation.
        /// Called by the interface.
        void handleEvent(const SimEvent &simEvent);

        /// Send an event to the simulation. Creates multiple events from a single event if configured so.
        /// Called by the DuT connector itself.
        /// \param simEvent Event that should be send.
        void sendEventToSim(const SimEvent &simEvent);

    protected:
        /// Handles an single event asynchronously from the simulation.
        /// Called whenever a new event for the device arrives.
        /// Please override this methode!
        /// \param simEvent
        virtual void handleEventSingle(const SimEvent &simEvent) {};

    private:

        // determine if an event needs to be processed
        bool canHandleSimEvent(const SimEvent &simEvent);

        bool isPeriodicEnabled(const SimEvent &simEvent);

        void setupTimer(const SimEvent &simEvent);

        void enablePeriodicSending(const std::string& operation, int periodMs);

        std::shared_ptr<boost::asio::io_service> io;
        std::thread timerRunner;
;

        std::set<std::string> processableOperations;

        std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim;

        std::map<std::string, std::unique_ptr<sim_interface::PeriodicTimer>> periodicTimers;
        std::map<std::string, int> periodicIntervals;
        std::unique_ptr<PeriodicTimer> aliveTimer;
        bool periodicTimerEnabled;
    };
}

#endif //SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
