//
// Created by Lukas on 19.10.2021.
//

#include "DuT_Connector.h"

DuT_Connector::DuT_Connector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim) : queueDuTEventToSim(queueDuTEventToSim)
{

}

DuT_Info DuT_Connector::get_DuT_Info() {
    DuT_Info info(
            "Test Connector",
            0x01000000,
            "DuT_Connector please override!");
    return info;
}

void DuT_Connector::handleEvent(SimEvent &simEvent) {
    if(canHandleSimEvent(simEvent)){
        std::cout << simEvent << std::endl;
    }
}

bool DuT_Connector::canHandleSimEvent(SimEvent &simEvent) {
    return true;
}

void DuT_Connector::sendEventToSim(SimEvent &simEvent) {
    queueDuTEventToSim->push(simEvent);
}
