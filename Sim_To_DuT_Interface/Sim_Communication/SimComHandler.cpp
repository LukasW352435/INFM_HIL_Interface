//
// Created by Lukas on 19.10.2021.
//

#include "SimComHandler.h"

SimComHandler::SimComHandler(SharedQueue<SimEvent> &queueSimToInterface) : queueSimToInterface(queueSimToInterface)
{

}

void SimComHandler::run() {
    // TODO async receive events from the Simulation and send them to the interface
    SimEvent event("Test","Test","Test");
    sendEventToInterface(event);
}

void SimComHandler::sendEventToSim(SimEvent &event) {
    // TODO implementation of sending an event to the simulation
    std::cout << "Async Sending of Event..." << std::endl;
}

void SimComHandler::sendEventToInterface(SimEvent &event) {
    queueSimToInterface.push(event);
}
