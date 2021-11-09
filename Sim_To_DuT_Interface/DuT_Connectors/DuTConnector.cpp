//
// Created by Lukas on 19.10.2021.
//

#include "DuTConnector.h"

#include <utility>

DuTConnector::DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim)
        : queueDuTToSim(std::move(queueDuTEventToSim)) {
}

DuTInfo DuTConnector::getDuTInfo() {
    DuTInfo info(
            "Test Connector",
            0x01000000,
            "DuTConnector please override!");
    return info;
}

void DuTConnector::handleEvent(const SimEvent &simEvent) {
    if (canHandleSimEvent(simEvent)) {
        std::cout << simEvent << std::endl;
        // TODO process event async
    }
}

bool DuTConnector::canHandleSimEvent(const SimEvent &simEvent) {
    // TODO check the operation of the event and config and determine if the event needs to be processed
    return true;
}

void DuTConnector::sendEventToSim(const SimEvent &simEvent) {
    queueDuTToSim->push(simEvent);
}
