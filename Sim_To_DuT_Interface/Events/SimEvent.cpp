//
// Created by Lukas on 31.10.2021.
//

#include "SimEvent.h"

#include <utility>

SimEvent::SimEvent() : SimEvent("", "", "") {
}

SimEvent::SimEvent(std::string operation, std::string value, std::string origin) :
        operation(std::move(operation)),
        value(std::move(value)),
        origin(std::move(origin)) {
    current = std::time(nullptr);
}

std::ostream &operator<<(std::ostream &os, const SimEvent &simEvent) {
    os << "Operation: " << simEvent.operation << std::endl;
    os << "Value: " << simEvent.value << std::endl;
    os << "Origin: " << simEvent.origin << std::endl;
    os << "Current: " << std::ctime(&simEvent.current) << std::endl;
    return os;
}