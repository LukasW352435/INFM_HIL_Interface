/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
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
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Lukas Wagenlehner
 * @version 1.0
 */

#ifndef INFM_HIL_INTERFACE_SIMEVENT_H
#define INFM_HIL_INTERFACE_SIMEVENT_H

#include <ctime>
#include <map>
#include <string>
#include <iostream>
#include <boost/variant.hpp>

namespace sim_interface {
    /**
     * Event object used to communicate between simulation and DuTs.
     */
    class SimEvent {
    public:
        /**
         * Create a new event.
         */
        SimEvent();

        /**
         * Create a new event.
         * @param operation The operation of the event. Used by the DuTConnectors to determent if the event is for
         * the corresponding DuT.
         * @param value The value of the event.
         * @param origin The origin of the event.
         */
        SimEvent(std::string operation, boost::variant<int, double, std::string, std::vector<unsigned char>> value, std::string origin);

        /**
         * Operation of the event.
         */
        std::string operation;
        /**
         * Value of the event.
         */
        boost::variant<int, double, std::string, std::vector<unsigned char>> value;
        /**
         * Time when the event was created.
         */
        std::time_t current;
        /**
         * Origin of the event.
         */
        std::string origin;

        /**
         * Returns a string representation of the event.
         * @param os
         * @param simEvent
         * @return
         */
        friend std::ostream &operator<<(std::ostream &os, const SimEvent &simEvent);
    };
}


#endif //INFM_HIL_INTERFACE_SIMEVENT_H
