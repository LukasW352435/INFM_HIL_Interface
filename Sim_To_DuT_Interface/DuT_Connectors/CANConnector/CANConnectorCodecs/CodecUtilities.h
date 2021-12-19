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
 * @author Marco Keul
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_CODECUTILITIES_H
#define SIM_TO_DUT_INTERFACE_CODECUTILITIES_H

// System includes
#include <netinet/in.h>
#include <cstring>

namespace sim_interface::dut_connector::can {

    /**
     * <summary>
     * Implements some utilities for the codecs.
     * </summary>
     */
    class CodecUtilities {

    public:

        /**
         * Checks the endianness of the host.
         * True means that the host uses big endian.
         * False meas that the host uses little endian.
         *
         * @return The endianness of the system.
         */
        static bool checkBigEndianness();

        /**
         *  Converts the endianness of a int16_t, int32_t, uint16_t, uint32_t or uint64_t.
         *  Note: Since uint8_t is only one byte it doesn't need a conversion.
         *
         * @param number - The number that should be converted.
         *
         * @return The converted number.
         */
        static int16_t convertEndianness(int16_t number);

        static uint16_t convertEndianness(uint16_t number);

        static int32_t convertEndianness(int32_t number);

        static uint32_t convertEndianness(uint32_t number);

        static uint64_t convertEndianness(uint64_t number);
    };

}

#endif //SIM_TO_DUT_INTERFACE_CODECUTILITIES_H