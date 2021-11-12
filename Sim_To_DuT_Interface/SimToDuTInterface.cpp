//
// Created by Lukas on 19.10.2021.
//

#include "SimToDuTInterface.h"

namespace sim_interface {
    SimToDuTInterface::SimToDuTInterface() {
        queueDuTToSim = std::make_shared<SharedQueue<SimEvent>>();
        queueSimToInterface = std::make_shared<SharedQueue<SimEvent>>();
    }

    void SimToDuTInterface::addConnector(dut_connector::DuTConnector *duTConnector) {
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
        std::thread threadSimToInterface(&SimToDuTInterface::handleEventsFromSim, this);
        threadSimToInterface.detach();
        std::thread threadDuTToSim(&SimToDuTInterface::handleEventsFromDuT, this);
        threadDuTToSim.detach();
    }

    std::shared_ptr<SharedQueue<SimEvent>> SimToDuTInterface::getQueueDuTToSim() {
        return queueDuTToSim;
    }

    std::shared_ptr<SharedQueue<SimEvent>> SimToDuTInterface::getQueueSimToInterface() {
        return queueSimToInterface;
    }

    [[noreturn]] void SimToDuTInterface::handleEventsFromSim() {
        while (true) {
            SimEvent simEvent;
            if (queueSimToInterface->pop(simEvent)) {
                sendEventToConnector(simEvent);
            }
        }
    }

    [[noreturn]] void SimToDuTInterface::handleEventsFromDuT() {
        while (true) {
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
}
