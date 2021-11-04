//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_SIMTODUTINTERFACE_H
#define SIM_TO_DUT_INTERFACE_SIMTODUTINTERFACE_H

#include "DuT_Connectors/DuTConnector.h"
#include "Sim_Communication/SimComHandler.h"
#include <list>
#include <iterator>

class SimToDuTInterface {
public:
    SimToDuTInterface();
    void addConnector(DuTConnector* duTConnector);
    void setSimComHandler(SimComHandler *simComHandler);

    void run();

    friend std::ostream& operator << (std::ostream& os, const SimToDuTInterface& interface);
    SharedQueue<SimEvent> &getQueueDuTToSim();
    SharedQueue<SimEvent> &getQueueSimToInterface();
private:
    SharedQueue<SimEvent> queueDuTToSim;
    SharedQueue<SimEvent> queueSimToInterface;
    SimComHandler* simComHandler = nullptr;
    std::list<DuTConnector*> duTConnectors;

    void sendEventToConnector(SimEvent &event);

    [[noreturn]] void handleEventsFromSim();

    [[noreturn]] void handleEventsFromDuT();
};

#endif //SIM_TO_DUT_INTERFACE_SIMTODUTINTERFACE_H
