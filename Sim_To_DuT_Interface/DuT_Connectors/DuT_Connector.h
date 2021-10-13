//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUT_CONNECTOR_H
#define SIM_TO_DUT_INTERFACE_DUT_CONNECTOR_H

#include <string>
#include "DuT_Info.h"

class DuT_Connector {
public:
    DuT_Connector();
    DuT_Info get_DuT_Info();
};

#endif //SIM_TO_DUT_INTERFACE_DUT_CONNECTOR_H
