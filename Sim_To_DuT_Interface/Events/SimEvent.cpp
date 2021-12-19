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
#include <chrono>
#include <iomanip>
#include "EventVisitor.h"

namespace sim_interface {
    SimEvent::SimEvent() : SimEvent("", "", "") {
    }

    SimEvent::SimEvent(std::string operation, boost::variant<int, double, std::string> value, std::string origin) :
            operation(std::move(operation)),
            value(std::move(value)),
            origin(std::move(origin)) {
        std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
        std::chrono::system_clock::duration duration = time_point.time_since_epoch();
        int64_t timestamp = duration.count();
        std::time_t time_t = std::chrono::system_clock::to_time_t(time_point);
        std::tm *tm = std::localtime(&time_t);

        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %X") << "." << std::to_string((timestamp / 1000) % 1000000);

        current = ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SimEvent &simEvent) {
        os << "Operation: " << simEvent.operation << std::endl;
        os << "Value: " << boost::apply_visitor(EventVisitor(), simEvent.value) << std::endl;
        os << "Origin: " << simEvent.origin << std::endl;
        os << "Current: " << simEvent.current << std::endl;
        return os;
    }
}