//
// Created by Lukas on 19.10.2021.
//

#include "DuTInfo.h"

DuTInfo::DuTInfo(std::string name, int version, std::string description) :
        name(name),
        version(version),
        description(description)
{

}

std::ostream& operator << (std::ostream& os, const DuTInfo& duTInfo){
    os << duTInfo.name << std::endl
    << "Version: " << std::hex << duTInfo.version << std::endl
    << std::dec << duTInfo.description << std::endl;
    return os;
}
