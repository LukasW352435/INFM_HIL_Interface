//
// Created by Lukas on 19.10.2021.
//

#include "SimComHandler.h"

#include <utility>

SimComHandler::SimComHandler(std::shared_ptr<SharedQueue<std::shared_ptr<SimEvent>>> queueSimToInterface) : queueSimToInterface(std::move(queueSimToInterface))
{

}

void SimComHandler::run() {
    // TODO async receive events from the Simulation and send them to the interface
    SimEvent event("Test","Test","Test");
    sendEventToInterface(std::make_shared<SimEvent>(event));
}

void SimComHandler::sendEventToSim(std::shared_ptr<SimEvent> simEvent) {
    // TODO implementation of sending an event to the simulation
    std::cout << "Async Sending of Event..." << std::endl;
}

void SimComHandler::sendEventToInterface(std::shared_ptr<SimEvent> simEvent) {
    queueSimToInterface->push(simEvent);
}
