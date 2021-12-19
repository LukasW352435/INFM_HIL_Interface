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

#include "MessageWithTimestamp.h"

#include <chrono>
#include <iomanip>

using namespace dummy_dut::rest::model;

namespace dummy_dut::rest::messages {
    MessageWithTimestamp::MessageWithTimestamp(Message *message) {
        this->m_Key = message->getKey();
        this->m_Status = message->getStatus();
        std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
        std::chrono::system_clock::duration duration = time_point.time_since_epoch();
        int64_t timestamp = duration.count();
        std::time_t time_t = std::chrono::system_clock::to_time_t(time_point);
        std::tm *tm = std::localtime(&time_t);

        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %X") << "." << std::to_string((timestamp / 1000) % 1000000);

        this->timestamp = ss.str();
    }

    std::string MessageWithTimestamp::toTableEntry() {
        return R"(
                <tr>
                    <td>)" + this->m_Key + R"(</td>
                    <td>)" + this->m_Status + R"(</td>
                    <td>)" + this->timestamp + R"(</td>
                </tr>)";
    }

    std::string MessageWithTimestamp::toTableEntryWithoutNewline() {
        return "<tr><td>" + this->m_Key + "</td><td>" + this->m_Status + "</td><td>" + this->timestamp +
               "</td></tr>";
    }
}
