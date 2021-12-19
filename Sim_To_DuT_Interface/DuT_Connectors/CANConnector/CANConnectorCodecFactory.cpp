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
 * @author Marco Keul
 * @author Matthias Bank
 * @version 1.0
 */

// Project includes
#include "CANConnectorCodecFactory.h"

namespace sim_interface::dut_connector::can {

    CANConnectorCodec *CANConnectorCodecFactory::createCodec(const std::string &codecName) {

        // Check if the codec name is empty
        if (codecName.empty()) {
            InterfaceLogger::logMessage("CAN Connector: Codec name can not be empty", LOG_LEVEL::ERROR);
            throw std::invalid_argument("CAN Connector: Codec name can not be empty");
        }

        // Create the right codec based on the given codec name.
        // Add your new codec here.
        if (CODEC_NAME_BMW == codecName) {
            return new BmwCodec();
        } else {
            // Unknown CAN codec name
            InterfaceLogger::logMessage(
                    "CAN Connector: Unknown CAN codec name <" + codecName + ">. Could not create the CAN codec",
                    LOG_LEVEL::ERROR);
            throw std::invalid_argument(
                    "CAN Connector: Unknown CAN codec name <" + codecName + ">. Could not create the CAN codec");
        }

    }

}