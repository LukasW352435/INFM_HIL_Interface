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

    std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> EthernetPacket::ToMap() {
        std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>> map = {};
        map["sourceMAC"] = sourceMAC;
        map["destinationMAC"] = destinationMAC;
        map["payload"] = payload;
        return map;
    }
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

#include <map>
#include <string>
#include <iostream>
#include <boost/variant.hpp>

namespace sim_interface {
    /**
     * <summary>
     * Event object used to communicate between simulation and DuTs.
     * </summary>
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
        SimEvent(std::string operation, boost::variant<int, double, std::string> value, std::string origin);

        /**
         * Operation of the event.
         */
        std::string operation;
        /**
         * Value of the event.
         */
        boost::variant<int, double, std::string> value;
        /**
         * Time when the event was created.
         */
        std::string current;
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
