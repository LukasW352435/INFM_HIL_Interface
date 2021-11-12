//
// Created by Lukas on 19.10.2021.
//

#include "DuTInfo.h"

#include <utility>

DuTInfo::DuTInfo(std::string name, int version, std::string description) :
        name(std::move(name)),
        version(version),
        description(std::move(description)) {

}

std::ostream &operator<<(std::ostream &os, const DuTInfo &duTInfo) {
    os << duTInfo.name << std::endl
       << "Version: " << std::hex << duTInfo.version << std::endl
       << std::dec << duTInfo.description << std::endl;
    return os;
}
