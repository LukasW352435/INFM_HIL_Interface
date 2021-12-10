/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
 *
 * This file is part of "Sim To DuT Interface".
 *
 * "Sim To DuT Interface" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sim To DuT Interface" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Lukas Wagenlehner
 * @version 1.0
 */

#include "SimEvent.h"

#include <utility>
#include <EventVisitor.h>

namespace sim_interface {
    SimEvent::SimEvent() : SimEvent("", "", "") {
    }

    SimEvent::SimEvent(std::string operation, boost::variant<int, double, std::string, std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>> value, std::string origin) :
            operation(std::move(operation)),
            value(std::move(value)),
            origin(std::move(origin)) {
        current = std::time(nullptr);
    }

    std::ostream &operator<<(std::ostream &os, const SimEvent &simEvent) {
        os << "Operation: " << simEvent.operation << std::endl;
        os << "Value: " << boost::apply_visitor(EventVisitor(), simEvent.value) << std::endl;
        os << "Origin: " << simEvent.origin << std::endl;
        os << "Current: " << std::ctime(&simEvent.current) << std::endl;
        return os;
    }
}