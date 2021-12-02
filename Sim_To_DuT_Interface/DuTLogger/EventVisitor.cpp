/**
 * Implementation of EventVisitor.h
 *
 * @author Marco Keul
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
    while((index = str.find(fromThis, index)) != std::string::npos) {
        str.replace(index, fromThis.length(), toThis);

        // increase the index, so we don't always handle the same part
        // it has to be increased by the length of 'toThis'
        index += toThis.length();
    }
}
