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
#include "CANConnectorReceiveOperation.h"

namespace sim_interface::dut_connector::can {

    CANConnectorReceiveOperation::CANConnectorReceiveOperation(std::string operation,
                                                               bool isCANFD,
                                                               bool hasMask,
                                                               int maskLength,
                                                               __u8 *maskData) :
            operation(std::move(operation)),
            isCANFD(isCANFD),
            hasMask(hasMask),
            maskLength(maskLength) {

        // Assert that:
        if (this->hasMask) {

            // Note: We need to use CAN/CANFD_MAX_DLEN and not CAN/CANFD_MTU because the DLEN is
            // the max size of the data array and the MTU is the sizeof the struct as a whole.
            if (this->isCANFD) {

                // - the mask length is greater than zero and not greater than the CANFD MTU
                if (this->maskLength < 0 || CANFD_MAX_DLEN < this->maskLength) {
                    throw std::invalid_argument(
                            "CAN Connector Receive Operation: Mask must not be negative or exceed the CANFD frame length");
                }

            } else {

                // - the mask length is greater than zero and not greater than the CAN MTU
                if (this->maskLength < 0 || CAN_MAX_DLEN < this->maskLength) {
                    throw std::invalid_argument(
                            "CAN Connector Receive Operation: Mask must not be negative or exceed the CAN frame length");
                }

            }

        }

        // Check if we have a mask
        if (this->hasMask) {

            // Assert that the mask data is not a null pointer
            if (maskData == nullptr) {
                throw std::invalid_argument(
                        "CAN Connector Receive Operation: Mask must not be NULL for a receive operation that has a mask");
            }

            // Set the mask length and copy the mask data
            this->mask.len = maskLength;
            std::memcpy(reinterpret_cast<void *>(this->mask.data), maskData, maskLength);
        }

    }

}