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

#ifndef SIM_TO_DUT_INTERFACE_CONNECTORINFO_H
#define SIM_TO_DUT_INTERFACE_CONNECTORINFO_H

#include <string>
#include <iostream>

namespace sim_interface::dut_connector {
    /**
     * Some basic information about the connector device.
     */
    class ConnectorInfo {
    public:
        /**
         * The name of the connector.
         */
        std::string name;
        /**
         * The version of the connector.
         */
        int version;
        /**
         * A description of the connector.
         */
        std::string description;

        /**
         * Create a new connector info.
         * @param name The name of the connector.
         * @param version The version of the connector.
         * @param description A description of the connector.
         */
        ConnectorInfo(std::string name, int version, std::string description);

        /**
         * String representation of the connector info.
         * @param os
         * @param duTInfo
         * @return
         */
        friend std::ostream &operator<<(std::ostream &os, const ConnectorInfo &duTInfo);
    };
}
#endif //SIM_TO_DUT_INTERFACE_CONNECTORINFO_H
