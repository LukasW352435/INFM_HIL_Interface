//
// Created by Lukas on 19.10.2021.
//

#include "Sim_Com_Handler.h"

Sim_Com_Handler::Sim_Com_Handler()
{

}

void Sim_Com_Handler::send_Event_to_Sim(SimEvent &event) {
    std::cout << "Async Sending of Event..." << std::endl;
    // TODO implementation of sending an event to the simulation
}

void Sim_Com_Handler::send_Event_to_Interface(SimEvent &event) {
    queueEventToInterface->push(event);
}

void Sim_Com_Handler::run() {
    // TODO start thread and get events from the simulation
    // TODO send SimEvents to the interface
    // TODO check for events in the queue and send them to the simulation
    SimEvent event("Turn Light Right", "On", "Car 1");
    send_Event_to_Interface(event);
}