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

#include "boost/variant.hpp"

namespace sim_interface::dut_connector::v2x {
    class V2XVisitor : public boost::static_visitor<std::vector<unsigned char>> {

    public:

        std::vector<unsigned char> operator()(std::vector<unsigned char> val) const {
            return val;
        }

        std::vector<unsigned char> operator()(int i) const {
            return {};
        }

        std::vector<unsigned char> operator()(double d) const {
            return {};
        }

        std::vector<unsigned char> operator()(const std::string &str) const {
            return {};
        }
    };
}


#endif //SIM_TO_DUT_INTERFACE_V2XVISITOR_H
