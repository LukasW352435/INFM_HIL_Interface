//
// Created by Lukas on 19.10.2021.
//

#include "DuTConnector.h"

DuTConnector::DuTConnector(SharedQueue<SimEvent>& queueDuTEventToSim) : queueDuTToSim(queueDuTEventToSim)
{
}

DuTInfo DuTConnector::getDuTInfo() {
    DuTInfo info(
            "Test Connector",
            0x01000000,
            "DuTConnector please override!");
    return info;
}

void DuTConnector::handleEvent(SimEvent &simEvent) {
    if(canHandleSimEvent(simEvent)){
        std::cout << simEvent << std::endl;
        // TODO process event async
    }
}

bool DuTConnector::canHandleSimEvent(SimEvent &simEvent) {
    // TODO check the operation of the event and config and determine if the event needs to be processed
    return true;
}

void DuTConnector::sendEventToSim(SimEvent &simEvent) {
    queueDuTToSim.push(simEvent);
}
