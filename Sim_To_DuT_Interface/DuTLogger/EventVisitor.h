/**
 * EventVisitor.h
 *
 * Contains a util class for string cast operations.
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
        return str;
    }
};

#endif //SIM_TO_DUT_INTERFACE_EVENTVISITOR_H
