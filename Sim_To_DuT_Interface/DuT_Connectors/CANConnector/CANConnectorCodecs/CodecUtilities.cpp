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

// Project includes
#include "CodecUtilities.h"

namespace sim_interface::dut_connector::can {

    bool CodecUtilities::checkBigEndianness() {

        // Note: If the project is upgraded from C++17 to C++20
        // one could have also used std::endian instead.

        // Note: The htons function always converts safely to big endian.
        // No matter the fact if the host uses little- or big endian.
        uint16_t testNumberInHostOrder = 0x1122;
        uint16_t testNumberInBigEndian = htons(testNumberInHostOrder);

        if (testNumberInHostOrder == testNumberInBigEndian) {
            return true;
        } else {
            return false;
        }

    }

    int16_t CodecUtilities::convertEndianness(int16_t number) {

        uint8_t data[2] = {0};
        std::memcpy(&data, &number, sizeof(data));

        // Swap the first and the second byte.
        return (((uint16_t) data[1] << 0) |
                ((uint16_t) data[0] << 8));
    }

    uint16_t CodecUtilities::convertEndianness(uint16_t number) {

        uint8_t data[2] = {0};
        std::memcpy(&data, &number, sizeof(data));

        // Swap the first and the second byte.
        return (((uint16_t) data[1] << 0) | ((uint16_t) data[0] << 8));
    }

    int32_t CodecUtilities::convertEndianness(int32_t number) {

        uint8_t data[4] = {0};
        std::memcpy(&data, &number, sizeof(data));

        // Swap the:
        // - first and the fourth byte
        // - swap the second and the third byte
        return (((uint32_t) data[3] << 0) |
                ((uint32_t) data[2] << 8) |
                ((uint32_t) data[1] << 16) |
                ((uint32_t) data[0] << 24));
    }

    uint32_t CodecUtilities::convertEndianness(uint32_t number) {

        uint8_t data[4] = {0};
        std::memcpy(&data, &number, sizeof(data));

        // Swap the:
        // - first and the fourth byte
        // - swap the second and the third byte
        return (((uint32_t) data[3] << 0) |
                ((uint32_t) data[2] << 8) |
                ((uint32_t) data[1] << 16) |
                ((uint32_t) data[0] << 24));
    }

    uint64_t CodecUtilities::convertEndianness(uint64_t number) {

        uint8_t data[8] = {0};
        std::memcpy(&data, &number, sizeof(data));

        // Swap the
        // - first and the eight byte
        // - second and the seventh byte.
        // - third and the sixth byte
        // - fourth and the fifth byte
        return (((uint64_t) data[7] << 0) |
                ((uint64_t) data[6] << 8) |
                ((uint64_t) data[5] << 16) |
                ((uint64_t) data[4] << 24) |
                ((uint64_t) data[3] << 32) |
                ((uint64_t) data[2] << 40) |
                ((uint64_t) data[1] << 48) |
                ((uint64_t) data[0] << 56));
    }

}