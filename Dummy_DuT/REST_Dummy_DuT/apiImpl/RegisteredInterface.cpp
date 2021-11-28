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

namespace dummy_dut::rest::impl {
    bool RegisteredInterface::addInterfaceCallback(const std::string &callback) {
        if (callbacks.find(callback) != callbacks.end()) {
            return false;
        }
        callbacks.insert(callback);
        return true;
    }

    bool RegisteredInterface::removeInterfaceCallback(const std::string &callback) {
        if (callbacks.find(callback) != callbacks.end()) {
            callbacks.erase(callback);
            return true;
        }
        return false;
    }

    std::set<std::string> RegisteredInterface::getCallbacks() {
        return callbacks;
    }
}