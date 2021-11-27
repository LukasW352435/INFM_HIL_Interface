/**
 * Implementation of EventVisitor.h
 *
 * @author Marco Keul
 */

#include "EventVisitor.h"

/**
 * This function scans the string for special chars that can cause troubles in a CSV file.
 * After one of this character is found, it will be handled by the following rules:
 * <br>
 * <br>
 * 1. If the string contains a comma or double-quotes the string has to be encapsulated by two double-quotes <br>
 * 2. Each double-quote char will be replaced by a pair of double-quote
 *
 * @see
 * https://en.wikipedia.org/wiki/Comma-separated_values
 * @see
 * https://stackoverflow.com/questions/4617935/is-there-a-way-to-include-commas-in-csv-columns-without-breaking-the-formatting
 *
 * @param str   - Search for special characters in this string
 * @return a valid string which
*/
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

/**
 * This function replaces all occurrence of a specified string with another string.
 * It won't do anything if one of the parameters is empty.
 *
 * @param str       - this string will be modified by this function
 * @param fromThis  - search for this string and replace it
 * @param toThis    - replace it with this string
*/
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
