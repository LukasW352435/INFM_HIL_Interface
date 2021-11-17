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
    class SimToDuTInterface {
    public:
        SimToDuTInterface();
        ~SimToDuTInterface();

        void addConnector(dut_connector::DuTConnector *duTConnector);

        void setSimComHandler(SimComHandler *simComHandler);

        void run();

        friend std::ostream &operator<<(std::ostream &os, const SimToDuTInterface &interface);

        std::shared_ptr<SharedQueue<SimEvent>> getQueueDuTToSim();

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
