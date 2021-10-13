//
// Created by Lukas on 19.10.2021.
//

#include "Sim_To_DuT_Interface.h"

Sim_To_DuT_Interface::Sim_To_DuT_Interface() {

}

void Sim_To_DuT_Interface::add_Connector(DuT_Connector* duTConnector) {
    duTConnectors.push_back(duTConnector);
}

std::ostream& operator << (std::ostream& os, const Sim_To_DuT_Interface& interface){
    for (auto it = interface.duTConnectors.begin(); it != interface.duTConnectors.end(); ++it){
        os << (*it)->get_DuT_Info();
    }
    return os;
}