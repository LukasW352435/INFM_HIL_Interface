//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_SIM_TO_DUT_INTERFACE_H
#define SIM_TO_DUT_INTERFACE_SIM_TO_DUT_INTERFACE_H

#include "DuT_Connectors/DuT_Connector.h"
#include "Sim_Communication/Sim_Com_Handler.h"
#include <list>
#include <iterator>

class Sim_To_DuT_Interface {
public:
    Sim_To_DuT_Interface(Sim_Com_Handler simComHandler);
    void add_Connector(DuT_Connector* duTConnector);
    // Send an event form the simulation to the connector
    void send_Event_To_Connector(SimEvent &event);
    // Send an event from a connector to the simulation
    void send_Event_To_Sim(SimEvent &event);
    void run();
    friend std::ostream& operator << (std::ostream& os, const Sim_To_DuT_Interface& interface);
    std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim;
private:
    std::list<DuT_Connector*> duTConnectors;
    Sim_Com_Handler simComHandler;
};


#endif //SIM_TO_DUT_INTERFACE_SIM_TO_DUT_INTERFACE_H
