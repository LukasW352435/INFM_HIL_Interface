/**
 * HIL - V2X Connector
 * This the connector to interface with the V2X DuT
 *
 * Copyright (C) 2021 Franziska Ihrler
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
 * along with "HIL - REST Dummy Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Franziska Ihrler
 * @author Michael Schmitz
 * @version 1.0
 */


#ifndef SIM_TO_DUT_INTERFACE_V2XVISITOR_H
#define SIM_TO_DUT_INTERFACE_V2XVISITOR_H

#include <boost/variant.hpp>
#include <map>
#include <vector>
#include "../../DuTLogger/DuTLogger.h"

namespace sim_interface::dut_connector::v2x {
    /**
     * Visitor to extract map of type string, variant<string, vector<unsigned char>> from SimEvent.value, logs warnings if type is not the required map
     */
    class V2XVisitor
            : public boost::static_visitor<std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>> {

    public:
        std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>
        operator()(std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> val) const {
            return val;
        }

        std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> operator()(int i) const {
            DuTLogger::logMessage("V2XConnector: Visitor called on int instead of std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>", LOG_LEVEL::WARNING);
            return {};
        }

        std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> operator()(double d) const {
            DuTLogger::logMessage("V2XConnector: Visitor called on double instead of std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>", LOG_LEVEL::WARNING);
            return {};
        }

        std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>
        operator()(const std::string &str) const {
            DuTLogger::logMessage("V2XConnector: Visitor called on string instead of std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>", LOG_LEVEL::WARNING);
            return {};
        }
    };
}


#endif //SIM_TO_DUT_INTERFACE_V2XVISITOR_H
