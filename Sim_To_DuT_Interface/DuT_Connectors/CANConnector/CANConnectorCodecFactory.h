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

#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H

// Project includes
#include "CANConnectorCodec.h"
#include "InterfaceLogger.h"

// Codec includes.
#include "BmwCodec.h"

/**
 * The name of the BMW CAN codec.
 */
#define CODEC_NAME_BMW "BmwCodec"

namespace sim_interface::dut_connector::can {

    /**
     * <summary>
     * Creates the requested CAN codec.
     * </summary>
     */
    class CANConnectorCodecFactory {

    public:

        /**
         * Creates the requested CAN codec.
         *
         * @param codecName - The name of the codec that should be created.
         *
         * @return The CAN codec.
         */
        static CANConnectorCodec *createCodec(const std::string &codecName);
    };

}

#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H