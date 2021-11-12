/**
 * HIL - REST Dummy DuT
 * This the implementation of the API defined in REST_Dummy_DuT.yaml
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - REST Dummy DuT".
 *
 * "HIL - REST Dummy DuT" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - REST Dummy DuT" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - REST Dummy DuT".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @version 1.0
 */

#include "RegisteredInterface.h"

namespace thi::dummy_dut::messages {
    /**
     * Add the given URL to the list of callbacks, if not already present
     * @param callback URL to add
     * @return State of operation
     */
    bool RegisteredInterface::addInterfaceCallback(const std::string &callback) {
        if (callbacks.find(callback) != callbacks.end()) {
            return false;
        }
        callbacks.insert(callback);
        return true;
    }

    /**
     * Remove the given URL from the list of callbacks, if present
     * @param callback URL to remove
     * @return State of operation
     */
    bool RegisteredInterface::removeInterfaceCallback(const std::string &callback) {
        if (callbacks.find(callback) != callbacks.end()) {
            callbacks.erase(callback);
            return true;
        }
        return false;
    }

    /**
     * Get a list of all registered callbacks
     * @return set of strings, each representing one URL
     */
    std::set<std::string> RegisteredInterface::getCallbacks() {
        return callbacks;
    }
}