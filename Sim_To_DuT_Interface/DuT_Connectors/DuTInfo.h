//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUT_INFO_H
#define SIM_TO_DUT_INTERFACE_DUT_INFO_H

#include <string>
#include <iostream>

class DuT_Info {
public:
    std::string name;
    int version;
    std::string description;
    DuT_Info(std::string name, int version, std::string description);
    friend std::ostream& operator << (std::ostream& os, const DuT_Info& duTInfo);
};

#endif //SIM_TO_DUT_INTERFACE_DUT_INFO_H
