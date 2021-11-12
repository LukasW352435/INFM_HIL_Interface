//
// Created by Lukas on 19.10.2021.
//

#include "SimComHandler.h"

#include <utility>

SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface)
        : queueSimToInterface(std::move(queueSimToInterface)) {

}

void SimComHandler::run() {
    // TODO async receive events from the Simulation and send them to the interface
    SimEvent event("Test", "Test", "Test");
    sendEventToInterface(event);
}

void SimComHandler::sendEventToSim(const SimEvent &simEvent) {
    // TODO implementation of sending an event to the simulation
    std::cout << "Async Sending of Event..." << std::endl;
    std::cout << simEvent << "lol";
}

void SimComHandler::sendEventToInterface(const SimEvent &simEvent) {
    queueSimToInterface->push(simEvent);
}
