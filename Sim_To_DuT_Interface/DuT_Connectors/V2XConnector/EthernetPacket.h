/**
 * HIL - V2X Connector
 * This the connector to interface with the V2X DuT
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - V2X Connector".
 *
 * "HIL - V2X Connector"" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - V2X Connector"" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - V2X Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @author Franziska Ihrler
 * @version 1.0
 */


#ifndef SIM_TO_DUT_INTERFACE_ETHERNETPACKET_H
#define SIM_TO_DUT_INTERFACE_ETHERNETPACKET_H

#include <string>
#include <utility>
#include <vector>
#include <linux/if_ether.h>
#include <map>
#include <boost/variant.hpp>

namespace sim_interface::dut_connector::v2x {
    class EthernetPacket {
    public:
        /**
         * Create an ethernet packet with already parsed data from map with required keys sourceMAC, destinationMAC, payload
         *
         * @param text_archive boost::archive string containing sourceMAC, destinationMAC, length of payload and payload
         */
        explicit EthernetPacket(const std::string &text_archive);

        /**
         * Create an ethernet packet from the raw received data (extract source and destination MAC)
         *
         * @param rawData raw received data
         */
        explicit EthernetPacket(std::vector<unsigned char> rawData);

        /**
         * Encode bytes of payload as text archive (byte for byte)
         *
         * @return text archive of payload
         */
        std::string getPacketAsArchive();

        /**
         * Convert object to byte vector of the following structure:
         * Byte 0-5:   source MAC address
         * Byte 6-11:  destination MAC address
         * Byte 12+13: Ethernet Frame Type
         * Rest:       Payload
         *
         * @param ethernetFrameType two bytes to set the ethernet frame type with
         * @return vector of bytes
         */
        std::vector<unsigned char> toBytes(unsigned short ethernetFrameType);

    private:
        /**
         * source MAC of ethernet header
         */
        std::string sourceMAC;

        /**
         * destination MAC of ethernet header
         */
        std::string destinationMAC;

        /**
         * bytes to send after the ethernet header
         */
        std::vector<unsigned char> payload;

        /**
         * Extract the ethernet header from the first bytes (sizeof(ethhdr) = 13) of the raw data
         *
         * @param rawData vector of bytes containing ethernet header and payload
         */
        void extractEthernetHeader(std::vector<unsigned char> rawData);

        /**
         * Convert the given bytes to hex encoded string in format aa:bb:cc:dd:ee:ff
         *
         * @param bytes Bytes of MAC address to encode
         * @return hex encoded MAC
         */
        static std::string getHexEncodedMAC(unsigned char bytes[]);

        /**
         * Convert the given hex string in format aa:bb:cc:dd:ee:ff to byte array
         *
         * @param hex hex encoded MAC address
         * @return bytes of MAC address
         */
        static std::vector<unsigned char> getBytesOfHexEncodedMAC(std::string hex);
    };
}


#endif //SIM_TO_DUT_INTERFACE_ETHERNETPACKET_H
