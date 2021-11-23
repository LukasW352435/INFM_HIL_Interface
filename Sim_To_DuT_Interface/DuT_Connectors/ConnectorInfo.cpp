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

#include "ConnectorInfo.h"

#include <utility>

namespace sim_interface::dut_connector {
    ConnectorInfo::ConnectorInfo(std::string name, int version, std::string description) :
            name(std::move(name)),
            version(version),
            description(std::move(description)) {

    }

    std::ostream &operator<<(std::ostream &os, const ConnectorInfo &duTInfo) {
        os << duTInfo.name << std::endl
           << "Version: " << std::hex << duTInfo.version << std::endl
           << std::dec << duTInfo.description << std::endl;
        return os;
    }
}