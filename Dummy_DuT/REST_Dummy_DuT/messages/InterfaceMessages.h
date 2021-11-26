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

#ifndef DUMMY_REST_DUT_INTERFACEMESSAGES_H
#define DUMMY_REST_DUT_INTERFACEMESSAGES_H

#include <vector>
#include "MessageWithTimestamp.h"

namespace dummy_dut::rest::messages {
    class InterfaceMessages {
    public:
        /**
         * Adds the given message to the end of the list
         * @param message Message to add to list
         */
         void addMessage(MessageWithTimestamp message);

        /**
         * Returns the list of messages
         * @return vector of MessageWithTimestamp
         */
        std::vector<MessageWithTimestamp> getMessages();

    private:
        std::vector<MessageWithTimestamp> messages = std::vector<MessageWithTimestamp>();
    };
}

#endif //DUMMY_REST_DUT_INTERFACEMESSAGES_H
