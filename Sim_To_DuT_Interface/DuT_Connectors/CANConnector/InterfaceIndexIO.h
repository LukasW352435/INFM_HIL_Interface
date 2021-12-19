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

#ifndef SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H
#define SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H

// System includes
#include <string>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>

namespace sim_interface::dut_connector::can {

    /**
     * <summary>
     * I/O control command class for getting the interface index.
     * For further information see boost asio io_control.
     * </summary>
     */
    class InterfaceIndexIO {

    public:

        /**
        * Constructor.
        *
        * @param interfaceName - The name of the interface which index should be resolved.
        */
        explicit InterfaceIndexIO(std::string interfaceName);

        /**
        * Returns the POSIX ioctl() value for getting the interface index by name.
        *
        * @returns The value of SIOCGIFINDEX
        */
        static int name();

        /**
        * Returns the ifreq struct that contains the interface name.
        *
        * @return The ifreq with the name
        */
        void *data();

        /**
        * Returns the resolved interface index.
        *
        * @return The resolved interface index.
        */
        int index() const;

    private:

        ifreq ifr{}; /**< The ifreq struct used for ioctl */
    };

}

#endif //SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H