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
 * @version 1.0
 */

#include <sstream>
#include <iomanip>
#include "../../DuTLogger/DuTLogger.h"
#include "EthernetPacket.h"

namespace sim_interface::dut_connector::v2x {
    /**
     * Extract strings from boost variant type of map, log warnings if type is wrong
     */
    struct StringMapVisitor : public boost::static_visitor<std::string>
    {
        std::string operator()(std::string operand) const
        {
            return operand;
        }

        std::string operator()(const std::vector<unsigned char>& operand) const
        {
            DuTLogger::logMessage("V2XConnector: EthernetPacket: Visitor called on vector<unsigned char> instead of string", LOG_LEVEL::WARNING);
            return "";
        }
    };

    /**
     * Extract vector<unsigned char> from boost variant type of map, log warnings if type is wrong
     */
    struct VectorMapVisitor : public boost::static_visitor<std::vector<unsigned char>>
    {
        std::vector<unsigned char> operator()(const std::string& operand) const
        {
            DuTLogger::logMessage("V2XConnector: EthernetPacket: Visitor called on string instead of vector<unsigned char>", LOG_LEVEL::WARNING);
            return {};
        }

        std::vector<unsigned char> operator()(std::vector<unsigned char> operand) const
        {
            return operand;
        }
    };

    EthernetPacket::EthernetPacket(std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> map) {
        if (map.count("sourceMAC") == 1 && map.count("destinationMAC") == 1 && map.count("payload") == 1) {
            sourceMAC = boost::apply_visitor(StringMapVisitor(), map["sourceMAC"]);
            destinationMAC = boost::apply_visitor(StringMapVisitor(), map["destinationMAC"]);
            payload = boost::apply_visitor(VectorMapVisitor(), map["payload"]);
        } else {
            DuTLogger::logMessage("V2XConnector: EthernetPacket: Couldn't construct packet, missing entries in map (required: sourceMAC, destinationMAC, payload).", LOG_LEVEL::ERROR);
        }
    }

    EthernetPacket::EthernetPacket(std::vector<unsigned char> rawData) {
        extractEthernetHeader(rawData);
        payload = std::vector<unsigned char>(rawData.begin() + sizeof(ethhdr), rawData.end());
    }

    std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> EthernetPacket::ToMap() {
        std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> map = {};
        map["sourceMAC"] = sourceMAC;
        map["destinationMAC"] = destinationMAC;
        map["payload"] = payload;
        return map;
    }

    std::vector<unsigned char> EthernetPacket::ToBytes() {
        std::vector<unsigned char> bytes;
        auto sourceMACBytes = getBytesOfHexEncodedMAC(sourceMAC);
        auto destinationMACBytes = getBytesOfHexEncodedMAC(destinationMAC);
        for (unsigned char & byte : sourceMACBytes) {
            bytes.push_back(byte);
        }
        for (unsigned char & byte : destinationMACBytes) {
            bytes.push_back(byte);
        }
        bytes.push_back(0x00);
        bytes.push_back(0x00);
        bytes.insert(bytes.end(), payload.begin(), payload.end());
        return bytes;
    }

    void EthernetPacket::extractEthernetHeader(std::vector<unsigned char> rawData) {
        if (rawData.size() < sizeof(ethhdr)) {
            return;
        }
        auto header = (struct ethhdr *)(rawData.data());
        sourceMAC = getHexEncodedMAC(header->h_source);
        destinationMAC = getHexEncodedMAC(header->h_dest);
    }

    std::string EthernetPacket::getHexEncodedMAC(unsigned char bytes[]) {
        std::stringstream hex;
        hex << std::hex;
        for (int i = 0; i < ETH_ALEN; i++) {
            hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
            if (ETH_ALEN - i >  1) {
                hex << ':';
            }
        }
        return hex.str();
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