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

// Project includes
#include "InterfaceIndexIO.h"

namespace sim_interface::dut_connector::can {

    InterfaceIndexIO::InterfaceIndexIO(std::string interfaceName) : ifr() {
        std::strncpy(ifr.ifr_name, interfaceName.c_str(), IF_NAMESIZE);
    }

    int InterfaceIndexIO::name() {
        return SIOCGIFINDEX;
    }

    void *InterfaceIndexIO::data() {
        return &ifr;
    }

    int InterfaceIndexIO::index() const {
        return ifr.ifr_ifindex;
    }

}