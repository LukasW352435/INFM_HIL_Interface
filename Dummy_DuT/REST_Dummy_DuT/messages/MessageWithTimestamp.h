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

#ifndef DUMMY_REST_DUT_MESSAGEWITHTIMESTAMP_H
#define DUMMY_REST_DUT_MESSAGEWITHTIMESTAMP_H

#include "Message.h"

namespace dummy_dut::rest::messages {
    /**
     * <summary>
     * A single message received from the interface enriched with the timestamp of reception
     * </summary>
     */
    class MessageWithTimestamp : dummy_dut::rest::model::Message {
    public:
        /**
         * Create a MessageWithTimestamp from a Message object
         * Key and value are used and the current UTC time is used as timestamp.
         * @param message Data to create object from
         */
        explicit MessageWithTimestamp(dummy_dut::rest::model::Message *message);

        /**
         * Convert the message to a table entry for the dashboard (with indentation)
         * @return string representation of object
         */
        std::string toTableEntry();

        /**
         * Convert the message to a table entry without any indentation or newlines for the SSE stream
         * @return string representation of object in a single line
         */
        std::string toTableEntryWithoutNewline();

        std::string timestamp;
    };
}


#endif //DUMMY_REST_DUT_MESSAGEWITHTIMESTAMP_H
