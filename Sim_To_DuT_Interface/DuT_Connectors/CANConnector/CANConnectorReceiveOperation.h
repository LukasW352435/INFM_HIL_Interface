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

#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H

// System includes
#include <string>
#include <cstring>
#include <stdexcept>
#include <linux/can.h>
#include <linux/can/bcm.h>

namespace sim_interface::dut_connector::can {

    /**
    * <summary>
    * Contains the necessary information to configure a receive operation for
    * the configuration of the CAN Connector.
    * </summary>
    */
    class CANConnectorReceiveOperation {

    public:

        /**
         * Constructor for an receive operation.
         *
         * @param operation  - The operation name.
         * @param isCANFD    - Flag for CANFD frames.
         * @param hasMask    - Flag if a mask should be used.
         * @param maskLength - The length of the mask.
         * @param maskData   - The mask data that should be used.
         */
        CANConnectorReceiveOperation(std::string operation,
                                     bool isCANFD,
                                     bool hasMask,
                                     int maskLength = 0,
                                     __u8 *maskData = nullptr);

        std::string operation;            /**< The operation name.                                                       */
        bool isCANFD;                     /**< Flag for CANFD frames.                                                    */
        bool hasMask;                     /**< Flag if a mask should be used to filter for content changes in the frames.*/
        int maskLength;                   /**< The length of the mask data.                                              */
        struct canfd_frame mask = {0};    /**< The mask that should be used to filter for content changes in the frames. */
    };

}

#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H