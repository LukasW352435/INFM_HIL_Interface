/**
 * CAN Connector.
 * The Connector enables the communication over a CAN/CANFD interface.
 *
 * Copyright (C) 2021 Matthias Bank
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
 * along with "Sim To DuT Interface". If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Matthias Bank
 * @version 1.0
 */

// Project includes
#include "CANConnectorConfig.h"

namespace sim_interface::dut_connector::can {

    CANConnectorConfig::CANConnectorConfig(std::string interfaceName,
                                           std::string codecName,
                                           std::set<std::string> operations,
                                           std::map<canid_t, CANConnectorReceiveOperation> frameToOperation,
                                           std::map<std::string, CANConnectorSendOperation> operationToFrame,
                                           std::map<std::string, int> periodicOperations,
                                           bool periodicTimerEnabled) :
            ConnectorConfig(std::move(operations), std::move(periodicOperations), periodicTimerEnabled),
            interfaceName(std::move(interfaceName)),
            codecName(std::move(codecName)),
            frameToOperation(std::move(frameToOperation)),
            operationToFrame(std::move(operationToFrame)) {

        // Assert that the interface name is not empty
        if (this->interfaceName.empty()) {
            throw std::invalid_argument("CAN Connector Config: The interface name must not be empty in the config");
        }

        // Assert that the codec name is not empty
        if (this->codecName.empty()) {
            throw std::invalid_argument("CAN Connector Config: The codec name must not be empty in the config");
        }

        // Assert that at least one map has an entry
        if (this->frameToOperation.empty() && this->operationToFrame.empty()) {
            throw std::invalid_argument(
                    "CAN Connector Config: There should be at least one sendOperation or receiveOperation in the config");
        }

    }

}