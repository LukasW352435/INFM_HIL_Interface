//
// Created by Lukas on 19.10.2021.
//

#include "SimToDuTInterface.h"

SimToDuTInterface::SimToDuTInterface() {

}

void SimToDuTInterface::addConnector(DuTConnector* duTConnector) {
    duTConnectors.push_back(duTConnector);
}

void SimToDuTInterface::sendEventToConnector(SimEvent &event) {
    for(auto it = duTConnectors.begin(); it != duTConnectors.end(); it++){
        (*it)->handleEvent(event);
    }
}

std::ostream& operator << (std::ostream& os, const SimToDuTInterface& interface){
    int count = 0;
    for (auto it = interface.duTConnectors.begin(); it != interface.duTConnectors.end(); ++it){
        os << count++ << ". ";
        os << (*it)->getDuTInfo();
    }
    return os;
}

void SimToDuTInterface::run() {
    std::thread threadSimToInterface(&SimToDuTInterface::handleEventsFromSim, this);
    threadSimToInterface.detach();
    std::thread threadDuTToSim(&SimToDuTInterface::handleEventsFromDuT, this);
    threadDuTToSim.detach();
}

SharedQueue<SimEvent> &SimToDuTInterface::getQueueDuTToSim() {
    return queueDuTToSim;
}

SharedQueue<SimEvent> &SimToDuTInterface::getQueueSimToInterface() {
    return queueSimToInterface;
}

[[noreturn]] void SimToDuTInterface::handleEventsFromSim() {
    while (true){
        SimEvent simEvent;
        if(queueSimToInterface.pop(simEvent)){
            sendEventToConnector(simEvent);
        }
    }
}

[[noreturn]] void SimToDuTInterface::handleEventsFromDuT() {
    while (true){
        SimEvent simEvent;
        if(queueDuTToSim.pop(simEvent)){
            if(simComHandler != nullptr){
                simComHandler->sendEventToSim(simEvent);
            }
        }
    }
}

void SimToDuTInterface::setSimComHandler(SimComHandler *simComHandler) {
    SimToDuTInterface::simComHandler = simComHandler;
}

