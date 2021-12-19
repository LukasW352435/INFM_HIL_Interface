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

#include "SimToDuTInterface.h"

namespace sim_interface {
    SimToDuTInterface::SimToDuTInterface() {
        queueDuTToSim = std::make_shared<SharedQueue<SimEvent>>();
        queueSimToInterface = std::make_shared<SharedQueue<SimEvent>>();
    }

    void SimToDuTInterface::addConnector(std::shared_ptr<dut_connector::DuTConnector> duTConnector) {
        duTConnectors.push_back(duTConnector);
    }

    void SimToDuTInterface::sendEventToConnector(const SimEvent &simEvent) {
        for (auto &duTConnector: duTConnectors) {
            duTConnector->handleEvent(simEvent);
        }
    }

    std::ostream &operator<<(std::ostream &os, const SimToDuTInterface &interface) {
        int count = 0;
        for (auto duTConnector: interface.duTConnectors) {
            os << count++ << ". ";
            os << duTConnector->getConnectorInfo();
        }
        return os;
    }

    void SimToDuTInterface::run() {
        threadSimToInterface = std::thread(&SimToDuTInterface::handleEventsFromSim, this);
        threadDuTToSim = std::thread(&SimToDuTInterface::handleEventsFromDuT, this);
    }

    std::shared_ptr<SharedQueue<SimEvent>> SimToDuTInterface::getQueueDuTToSim() {
        return queueDuTToSim;
    }

    std::shared_ptr<SharedQueue<SimEvent>> SimToDuTInterface::getQueueSimToInterface() {
        return queueSimToInterface;
    }

    void SimToDuTInterface::handleEventsFromSim() {
        while (stopThreads) {
            SimEvent simEvent;
            if (queueSimToInterface->pop(simEvent)) {
                sendEventToConnector(simEvent);
            }
        }
    }

    void SimToDuTInterface::handleEventsFromDuT() {
        while (stopThreads) {
            SimEvent simEvent;
            if (queueDuTToSim->pop(simEvent)) {
                if (simComHandler != nullptr) {
                    simComHandler->sendEventToSim(simEvent);
                }
            }
        }
    }

    void SimToDuTInterface::setSimComHandler(SimComHandler *simComHandler) {
        SimToDuTInterface::simComHandler = simComHandler;
    }

    SimToDuTInterface::~SimToDuTInterface() {
        stopThreads = false;
        queueDuTToSim->Stop();
        queueSimToInterface->Stop();
        threadSimToInterface.join();
        threadDuTToSim.join();
    }
}
