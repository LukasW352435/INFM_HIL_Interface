/**
 * HIL - DuT Connector
 * This the base connector to interface with DuTs
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - DuT Connector".
 *
 * "HIL - REST Dummy DuT" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - DuT Connector" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - DuT Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_CONNECTORCONFIG_H
#define SIM_TO_DUT_INTERFACE_CONNECTORCONFIG_H

#include <set>
#include <map>
#include <string>
#include <stdexcept>

namespace sim_interface::dut_connector {
    /**
     * <summary>
     * Configuration for a single DuTConnector
     * </summary>
     * Base class of the Configs for any connector.
     * Supported operations and operations to be repeated with corresponding intervals,
     * as well as if a software timer is needed can be configured.
     */
    class ConnectorConfig {
    public:
        explicit ConnectorConfig(std::set<std::string> operations, std::map<std::string, int> periodicOperations = {},
                                 bool periodicTimerEnabled = false)
                : operations(std::move(operations)), periodicOperations(std::move(periodicOperations)),
                  periodicTimerEnabled(periodicTimerEnabled) {
            if (this->operations.empty()) {
                throw std::invalid_argument("Set of operations cannot be empty");
            }
            for (const auto &periodicOperation: this->periodicOperations) {
                if (this->operations.find(periodicOperation.first) == this->operations.end()) {
                    throw std::invalid_argument("Periodic operation not found in operations");
                };
            }
        };

        /** Default destructor */
        virtual ~ConnectorConfig() = default;

        /** Set of processable operations */
        std::set<std::string> operations{};

        /** Map of operations to be periodically repeated with interval as value */
        std::map<std::string, int> periodicOperations{};

        /** Enable periodic timer on Connector level */
        bool periodicTimerEnabled = false;
    };
}


#endif //SIM_TO_DUT_INTERFACE_CONNECTORCONFIG_H
