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

#ifndef SIM_TO_DUT_INTERFACE_SIMTODUTINTERFACE_H
#define SIM_TO_DUT_INTERFACE_SIMTODUTINTERFACE_H

#include "DuT_Connectors/DuTConnector.h"
#include "Sim_Communication/SimComHandler.h"
#include <list>
#include <iterator>

namespace sim_interface {
    /// A Interface between a simulation and multiple DuT devices.
    class SimToDuTInterface {
    public:
        /// Create a new interface.
        SimToDuTInterface();

        /// Destroys the interface, stops all threads and queues.
        ~SimToDuTInterface();

        /// Add DuT connectors to the interface.
        /// \param duTConnector A DuT connector that has derived form the DuTConnector class.
        void addConnector(dut_connector::DuTConnector *duTConnector);

        /// Sets the handler that connects to the Simulation.
        /// \param simComHandler A communication handler to communicate with the simulation.
        void setSimComHandler(SimComHandler *simComHandler);

        /// Starts the interface.
        void run();

        /// Returns a string representation for the interface.
        /// \param os
        /// \param interface
        /// \return
        friend std::ostream &operator<<(std::ostream &os, const SimToDuTInterface &interface);

        /// Queue used to "send" SimEvents from the DuT devices to the simulation.
        /// \return A thread save SharedQueue.
        std::shared_ptr<SharedQueue<SimEvent>> getQueueDuTToSim();

        /// Queue used to "send" SimEvents from the simulation to the interface.
        /// \return A thread save SharedQueue.
        std::shared_ptr<SharedQueue<SimEvent>> getQueueSimToInterface();

    private:
        std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim;
        std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface;
        SimComHandler *simComHandler = nullptr;
        std::list<dut_connector::DuTConnector *> duTConnectors;
        std::thread threadSimToInterface;
        std::thread threadDuTToSim;
        bool stopThreads = true;

        void sendEventToConnector(const SimEvent &event);

        void handleEventsFromSim();

        void handleEventsFromDuT();
    };
}

#endif //SIM_TO_DUT_INTERFACE_SIMTODUTINTERFACE_H
