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

#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H

// Project includes
#include "../../Events/SimEvent.h"
#include "InterfaceLogger.h"

// System includes
#include <vector>
#include <linux/can.h>

namespace sim_interface::dut_connector::can {

    /**
    * <summary>
    * The CAN Connector Codec defines the interface that each codec implementation must fulfill.
    * </summary>
    */
    class CANConnectorCodec {

    public:

        /**
        * Converts an simulation event to a CAN/CANFD payload and determines the sendOperation name.
        *
        * @param event - The simulation event we want to transform into a CAN/CANFD frame payload.
        *
        * @return The CAN/CANFD frame payload and the sendOperation name.
        */
        virtual std::pair<std::vector<__u8>, std::string> convertSimEventToFrame(SimEvent event) = 0;

        /**
         * Converts a CAN/CANFD frame to the corresponding simulation events.
         *
         * @param frame   - The frame that we want to transform.
         * @param isCanfd - Flag for CANFD frames.
         *
         * @return The simulation events that were contained in the CAN/CANFD frame.
         */
        virtual std::vector<SimEvent> convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd) = 0;
    };

}

#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H