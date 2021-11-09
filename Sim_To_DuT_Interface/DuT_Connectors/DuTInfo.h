//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUTINFO_H
#define SIM_TO_DUT_INTERFACE_DUTINFO_H

#include <string>
#include <iostream>

class DuTInfo {
public:
    std::string name;
    int version;
    std::string description;
    DuTInfo(std::string name, int version, std::string description);
    friend std::ostream& operator << (std::ostream& os, const DuTInfo& duTInfo);
};

#endif //SIM_TO_DUT_INTERFACE_DUTINFO_H
