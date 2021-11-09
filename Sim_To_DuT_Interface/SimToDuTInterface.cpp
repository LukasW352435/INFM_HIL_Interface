//
// Created by Lukas on 19.10.2021.
//

#include "SimToDuTInterface.h"

SimToDuTInterface::SimToDuTInterface() {
    queueDuTToSim = std::make_shared<SharedQueue<std::shared_ptr<SimEvent>>>();
    queueSimToInterface = std::make_shared<SharedQueue<std::shared_ptr<SimEvent>>>();
}

void SimToDuTInterface::addConnector(DuTConnector* duTConnector) {
    duTConnectors.push_back(duTConnector);
}

void SimToDuTInterface::sendEventToConnector(std::shared_ptr<SimEvent> simEvent) {
    for(auto & duTConnector : duTConnectors){
        duTConnector->handleEvent(simEvent);
    }
}

std::ostream& operator << (std::ostream& os, const SimToDuTInterface& interface){
    int count = 0;
    for (auto duTConnector : interface.duTConnectors){
        os << count++ << ". ";
        os << duTConnector->getDuTInfo();
    }
    return os;
}

void SimToDuTInterface::run() {
    std::thread threadSimToInterface(&SimToDuTInterface::handleEventsFromSim, this);
    threadSimToInterface.detach();
    std::thread threadDuTToSim(&SimToDuTInterface::handleEventsFromDuT, this);
    threadDuTToSim.detach();
}

std::shared_ptr<SharedQueue<std::shared_ptr<SimEvent>>> SimToDuTInterface::getQueueDuTToSim() {
    return queueDuTToSim;
}

std::shared_ptr<SharedQueue<std::shared_ptr<SimEvent>>> SimToDuTInterface::getQueueSimToInterface() {
    return queueSimToInterface;
}

[[noreturn]] void SimToDuTInterface::handleEventsFromSim() {
    while (true){
        std::shared_ptr<SimEvent> simEvent;
        if(queueSimToInterface->pop(simEvent)){
            sendEventToConnector(simEvent);
        }
    }
}

[[noreturn]] void SimToDuTInterface::handleEventsFromDuT() {
    while (true){
        std::shared_ptr<SimEvent> simEvent;
        if(queueDuTToSim->pop(simEvent)){
            if(simComHandler != nullptr){
                simComHandler->sendEventToSim(simEvent);
            }
        }
    }
}

void SimToDuTInterface::setSimComHandler(SimComHandler *simComHandler) {
    SimToDuTInterface::simComHandler = simComHandler;
}

