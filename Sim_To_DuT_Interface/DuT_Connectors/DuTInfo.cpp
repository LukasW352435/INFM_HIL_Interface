//
// Created by Lukas on 19.10.2021.
//

#include "DuT_Info.h"

DuT_Info::DuT_Info(std::string name, int version, std::string description) :
        name(name),
        version(version),
        description(description)
{

}

std::ostream& operator << (std::ostream& os, const DuT_Info& duTInfo){
    os << duTInfo.name << std::endl
    << "Version: " << std::hex << duTInfo.version << std::endl
    << std::dec << duTInfo.description << std::endl;
    return os;
}
