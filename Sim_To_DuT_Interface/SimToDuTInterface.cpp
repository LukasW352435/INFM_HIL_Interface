//
// Created by Lukas on 19.10.2021.
//

#include "Sim_To_DuT_Interface.h"

Sim_To_DuT_Interface::Sim_To_DuT_Interface(Sim_Com_Handler simComHandler) : simComHandler(simComHandler) {

}

void Sim_To_DuT_Interface::add_Connector(DuT_Connector* duTConnector) {
    duTConnectors.push_back(duTConnector);
}

void Sim_To_DuT_Interface::send_Event_To_Connector(SimEvent &event) {
    for(auto it = duTConnectors.begin(); it != duTConnectors.end(); it++){
        (*it)->handleEvent(event);
    }
}

std::ostream& operator << (std::ostream& os, const Sim_To_DuT_Interface& interface){
    for (auto it = interface.duTConnectors.begin(); it != interface.duTConnectors.end(); ++it){
        os << (*it)->get_DuT_Info();
    }
    return os;
}

void Sim_To_DuT_Interface::run() {

}

void Sim_To_DuT_Interface::send_Event_To_Sim(SimEvent &event) {

}
