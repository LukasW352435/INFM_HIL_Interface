/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Marco Keul
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
 * @author Marco Keul
 * @version 1.0
 */

#include "EventVisitor.h"

std::string EventVisitor::checkForSpecialChars(const std::string &str) {
    // search for special chars
    if (str.find(',') != std::string::npos || str.find('"') != std::string::npos) {
        // we found such chars!
        // we are not allowed to change the arguments string (of the function), so get a new one
        std::string result = str;

        // search for double-quotes (it's still possible, that this char is not in the string)
        // replace all double-quotes with a pair of double-quotes
        if (str.find('"') != std::string::npos) {
            // found one -> replace it with a pair of this char
            replaceAll(result, "\"", "\"\"");
        }

        // we have to put a double-quote at the start and the end of the string
        return ('"' + result + '"');
    }

    // no need to change anything on the string
    return str;
}

void EventVisitor::replaceAll(std::string &str, const std::string &fromThis, const std::string &toThis) {
    // check for invalid arguments
    if (fromThis.empty() || toThis.empty()) {
        return;
    }

    // declare an index to handle the replacement logic
    size_t index = 0;

    // if the string contains the part which should be replaced, there will be work for us.
    // we have to do this, for every appearance of 'fromThis'
    while ((index = str.find(fromThis, index)) != std::string::npos) {
        str.replace(index, fromThis.length(), toThis);

        // increase the index, so we don't always handle the same part
        // it has to be increased by the length of 'toThis'
        index += toThis.length();
    }
}
