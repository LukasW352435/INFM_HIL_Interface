//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_SIM_TO_DUT_INTERFACE_H
#define SIM_TO_DUT_INTERFACE_SIM_TO_DUT_INTERFACE_H

#include "DuT_Connectors/DuT_Connector.h"
#include <list>
#include <iterator>

class Sim_To_DuT_Interface {
public:
    Sim_To_DuT_Interface();
    void add_Connector(DuT_Connector* duTConnector);
    friend std::ostream& operator << (std::ostream& os, const Sim_To_DuT_Interface& interface);
private:
    std::list<DuT_Connector*> duTConnectors;
};


#endif //SIM_TO_DUT_INTERFACE_SIM_TO_DUT_INTERFACE_H
