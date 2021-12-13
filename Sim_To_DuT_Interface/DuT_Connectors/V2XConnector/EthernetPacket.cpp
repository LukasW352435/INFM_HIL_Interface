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

#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../../DuTLogger/DuTLogger.h"
#include "EthernetPacket.h"

namespace sim_interface::dut_connector::v2x {
    EthernetPacket::EthernetPacket(const std::string& text_archive) {
        int payload_length;
        try {
            std::stringstream ss(text_archive);
            boost::archive::text_iarchive ar(ss);
            ar >> sourceMAC;
            ar >> destinationMAC;
            ar >> payload_length;
            unsigned char byte;
            for (int i = 0; i < payload_length; i++) {
                ar >> byte;
                payload.push_back(byte);
            }
        } catch (boost::archive::archive_exception &_) {
            DuTLogger::logMessage("V2XConnector: EthernetPacket: Couldn't construct packet, malformed text_archive.", LOG_LEVEL::ERROR);
        }
    }

    EthernetPacket::EthernetPacket(std::vector<unsigned char> rawData) {
        payload = std::vector<unsigned char>(rawData.begin(), rawData.end());
    }

    std::string EthernetPacket::getPayloadAsArchive() {
        std::stringstream ss;

        boost::archive::text_oarchive ar(ss);
        ar << payload.size();
        for (const auto& byte : payload)
        {
            ar << byte;
        }
        return ss.str();
    }

    std::vector<unsigned char> EthernetPacket::toBytes(unsigned short ethernetFrameType) {
        std::vector<unsigned char> bytes;
        auto sourceMACBytes = getBytesOfHexEncodedMAC(sourceMAC);
        auto destinationMACBytes = getBytesOfHexEncodedMAC(destinationMAC);
        for (unsigned char & byte : destinationMACBytes) {
            bytes.push_back(byte);
        }
        for (unsigned char & byte : sourceMACBytes) {
            bytes.push_back(byte);
        }
        bytes.push_back((unsigned char) ethernetFrameType >> 8);
        bytes.push_back((unsigned char) ethernetFrameType);
        bytes.insert(bytes.end(), payload.begin(), payload.end());
        return bytes;
    }

    std::vector<unsigned char> EthernetPacket::getBytesOfHexEncodedMAC(std::string hex) {
        if (hex.length() != ETH_ALEN * 2 + ETH_ALEN - 1) {
            DuTLogger::logMessage("V2XConnector: EthernetPacket: Wrong length for hex string, required length is 2*ETH_ALEN + ETH_ALEN - 1", LOG_LEVEL::ERROR);
            return {0, 0, 0, 0, 0, 0};
        }
        std::replace(hex.begin(), hex.end(), ':', ' ');
        std::istringstream hex_stream(hex);
        std::vector<unsigned char> bytes;
        unsigned int c;
        while (hex_stream >> std::hex >> c)
        {
            bytes.push_back(c);
        }
        return bytes;
    }
}