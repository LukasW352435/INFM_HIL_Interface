//
// Created by Lukas on 31.10.2021.
//

#ifndef INFM_HIL_INTERFACE_SIMEVENT_H
#define INFM_HIL_INTERFACE_SIMEVENT_H

#include <ctime>
#include <string>
#include <iostream>

class SimEvent {
public:
    SimEvent();

    SimEvent(std::string operation, std::string value, std::string origin);

    // change operation string to enum?
    std::string operation;
    std::string value;
    std::time_t current;
    std::string origin;

    friend std::ostream &operator<<(std::ostream &os, const SimEvent &simEvent);
};


#endif //INFM_HIL_INTERFACE_SIMEVENT_H
