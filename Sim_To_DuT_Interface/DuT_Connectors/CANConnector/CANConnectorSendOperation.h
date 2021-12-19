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

#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H

// System includes
#include <string>
#include <stdexcept>
#include <linux/can.h>
#include <linux/can/bcm.h>

namespace sim_interface::dut_connector::can {

    /**
     * <summary>
     * Contains the necessary information to configure a send operation for
     * the configuration of the CAN Connector.
     * </summary>
     */
    class CANConnectorSendOperation {

    public:

        /**
         * Constructor
         *
         * @param canID     - The CAN ID of the frame.
         * @param isCANFD   - Flag for CANFD frames.
         * @param isCyclic  - Flag for cyclic frames.
         * @param announce  - Flag for immediately sending out updates once.
         * @param count     - Number of times the frame is send with the first interval.
         * @param ival1     - First Interval.
         * @param ival2     - Second Interval.
         */
        CANConnectorSendOperation(canid_t canID,
                                  bool isCANFD,
                                  bool isCyclic,
                                  bool announce = false,
                                  __u32 count = 0,
                                  struct bcm_timeval ival1 = {0},
                                  struct bcm_timeval ival2 = {0});

        // Data members
        canid_t canID;                  /**< The CAN ID of the frame.                                   */
        bool isCANFD;                   /**< Flag for CANFD frames.                                     */
        bool isCyclic;                  /**< Flag for cyclic messages.                                  */
        bool announce;                  /**< Flag for immediately sending out updates once.             */
        __u32 count;                    /**< Number of times the frame is send with the first interval. */
        struct bcm_timeval ival1;       /**< First Interval.                                            */
        struct bcm_timeval ival2;       /**< Second Interval.                                           */
    };

}

#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H