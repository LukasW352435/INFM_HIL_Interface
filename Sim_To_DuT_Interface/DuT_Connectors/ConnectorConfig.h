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
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <cassert>

namespace sim_interface::dut_connector {
    class ConnectorConfig {
    private:

    public:
        explicit ConnectorConfig(std::set<std::string> operations, std::string connectorType, std::map<std::string, int> periodicOperations = {}, bool periodicTimerEnabled = false)
        : operations(std::move(operations)), connectorType(connectorType), periodicOperations(std::move(periodicOperations)), periodicTimerEnabled(periodicTimerEnabled) {

            assert(!this->operations.empty()); // Set of operations cannot be empty
            for(const auto& periodicOperation : this->periodicOperations) {
                assert(this->operations.find(periodicOperation.first) != this->operations.end()); // Periodic operation not found in operations
            }
        };
        virtual ~ConnectorConfig() = default;


        /** Set of processable operations */
        std::set<std::string> operations{};

        /** Map of operations to be periodically repeated with interval as value */
        std::map<std::string, int> periodicOperations{};

        /** Enable periodic timer on Connector level */
        bool periodicTimerEnabled = false;

        std::string connectorType;
    };

}


#endif //SIM_TO_DUT_INTERFACE_CONNECTORCONFIG_H
