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
#include "CANConnectorSendOperation.h"

namespace sim_interface::dut_connector::can {

    CANConnectorSendOperation::CANConnectorSendOperation(canid_t canID,
                                                         bool isCANFD,
                                                         bool isCyclic,
                                                         bool announce,
                                                         __u32 count,
                                                         struct bcm_timeval ival1,
                                                         struct bcm_timeval ival2) :
            canID(canID),
            isCANFD(isCANFD),
            isCyclic(isCyclic),
            announce(announce),
            count(count),
            ival1(ival1),
            ival2(ival2) {

        // Check if it is a cyclic operation
        if (this->isCyclic) {

            // Assert that the elements of ival1 and ival2 are not negative
            if (this->ival1.tv_sec < 0 || this->ival1.tv_usec < 0 || this->ival2.tv_sec < 0 ||
                this->ival2.tv_usec < 0) {
                throw std::invalid_argument("CAN Connector Send Operation: ival value must not be negative");
            }

            // Assert: that at least one value of ival2 is greater than zero
            if (this->count == 0) {

                // Note: If count is zero only ival2 is being used
                // - that at least one value of ival2 is greater than zero
                if (this->ival2.tv_sec == 0 && this->ival2.tv_usec == 0) {
                    throw std::invalid_argument(
                            "CAN Connector Send Operation: ival must contain at least one value greater than zero");
                }

            } else {

                // - that at least one value of each ival is greater than zero
                if ((this->ival1.tv_sec == 0 && this->ival1.tv_usec == 0) ||
                    (this->ival2.tv_sec == 0 && this->ival2.tv_usec == 0)) {
                    throw std::invalid_argument(
                            "CAN Connector Send Operation: ival must contain at least one value greater than zero");
                }

            }

        }

    }

}