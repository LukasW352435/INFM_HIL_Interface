//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H

#include <string>
#include <iostream>
#include <set>
#include <memory>
#include "../DuT_Connectors/DuTInfo.h"
#include "../Events/SimEvent.h"
#include "../Utility/SharedQueue.h"

class DuTConnector {
public:
    DuTConnector(SharedQueue<SimEvent> &queueDuTToSim);
    // return some basic information from the connector
    virtual DuTInfo getDuTInfo();
    // handle event from the simulation async
    virtual void handleEvent(SimEvent &simEvent);
private:
    std::set<std::string> processableOperations;
    // determine if an event needs to be processed
    bool canHandleSimEvent(SimEvent &simEvent);
    // send the event to the simulation
    void sendEventToSim(SimEvent &simEvent);
    SharedQueue<SimEvent>& queueDuTToSim;
};

#endif //SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
