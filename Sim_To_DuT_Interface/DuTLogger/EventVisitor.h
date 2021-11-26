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
        return checkForBadChars(str);
    }

    std::string checkForBadChars(const std::string &str) const {
        if (str.find('\"') != std::string::npos) {
            std::string res = str;
            replaceAll(res, "\"", "\"\"\"");
            return res;
        }

        if (str.find(',') != std::string::npos) {
            std::string res = str;
            replaceAll(res, ",", "\",\"");
            return res;
        }
        return str;
    }

    void replaceAll(std::string &str, const std::string &fromThis, const std::string &toThis) const {
        // check for invalid arguments
        if (fromThis.empty() || toThis.empty()) {
            return;
        }

        // declare an index to handle the replacement logic
        size_t index = 0;

        // if the string contains the part which should be replaced, there will be work for us
        // we have to do this, for every appearance of this part
        while((index = str.find(fromThis, index)) != std::string::npos) {
            str.replace(index, fromThis.length(), toThis);

            // increase the index, so we don't always handle the same part
            // it has to be increased by the length of 'toThis'
            index += toThis.length();
        }
    }
};

#endif //SIM_TO_DUT_INTERFACE_EVENTVISITOR_H
