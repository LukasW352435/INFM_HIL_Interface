//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUT_CONNECTOR_H
#define SIM_TO_DUT_INTERFACE_DUT_CONNECTOR_H

#include <string>
#include <iostream>
#include <set>
#include <memory>
#include "../DuT_Connectors/DuT_Info.h"
#include "../Events/SimEvent.h"
#include "../Utility/SharedQueue.h"

class DuT_Connector {
public:
    DuT_Connector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim);
    DuT_Info get_DuT_Info();
    virtual void handleEvent(SimEvent &simEvent);
private:
    std::set<std::string> processableOperations;
    virtual bool canHandleSimEvent(SimEvent &simEvent);
    void sendEventToSim(SimEvent &simEvent);
    std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim;
};

#endif //SIM_TO_DUT_INTERFACE_DUT_CONNECTOR_H
