/**
 * EventVisitor.h
 *
 * Contains a util class for string cast operations.
 *
 * @author Marco Keul
 */

#ifndef SIM_TO_DUT_INTERFACE_EVENTVISITOR_H
#define SIM_TO_DUT_INTERFACE_EVENTVISITOR_H

#include <string>
#include <map>
#include <sstream>
#include "boost/variant.hpp"

/**
 * Class to convert a value, that is allowed to have different types, into a string value.
 */
class EventVisitor : public boost::static_visitor<std::string> {

public:
    std::string operator() (int i) const {
        return std::to_string(i);
    }

    std::string operator() (double d) const {
        return std::to_string(d);
    }

    std::string operator() (const std::string &str) const {
        return checkForSpecialChars(str);
    }

    /**
     * This function scans the string for special chars that can cause troubles in a CSV file.
     * After one of this character is found, it will be handled by the following rules:
     *
     * 1. If the string contains a comma or double-quotes the string has to be encapsulated by two double-quotes
     * 2. Each double-quote char will be replaced by a pair of double-quote
     *
     * @see https://en.wikipedia.org/wiki/Comma-separated_values
     * @see https://stackoverflow.com/questions/4617935/is-there-a-way-to-include-commas-in-csv-columns-without-breaking-the-formatting
     *
     * @param str   - Search for special characters in this string
     * @return a valid string which
     */
    static std::string checkForSpecialChars(const std::string &str);

    /**
     * This function replaces all occurrence of a specified string with another string.
     * It won't do anything if one of the parameters is empty.
     *
     * @param str       - this string will be modified by this function
     * @param fromThis  - search for this string and replace it
     * @param toThis    - replace it with this string
     */
    static void replaceAll(std::string &str, const std::string &fromThis, const std::string &toThis);
};

#endif //SIM_TO_DUT_INTERFACE_EVENTVISITOR_H
