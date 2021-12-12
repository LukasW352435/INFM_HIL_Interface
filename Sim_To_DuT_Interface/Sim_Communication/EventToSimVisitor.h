/**
 * SimEvent Visitor for serializing SimEvents for sending over zeroMQ
 *
 * Copyright (C) 2021 Michael Schmitz
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
 * @author Michael Schmitz
 * @version 1.0
 */


#ifndef SIM_TO_DUT_INTERFACE_EVENTTOSIMVISITOR_H
#define SIM_TO_DUT_INTERFACE_EVENTTOSIMVISITOR_H

#include <string>
#include <map>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "boost/variant.hpp"

/**
 * Class to convert a value, that is allowed to have different types, into a string value.
 */

namespace sim_interface {
class EventToSimVisitor : public boost::static_visitor<std::string> {

public:
    std::string operator() (int i) const {
        return std::to_string(i);
    }

    std::string operator() (double d) const {
        return std::to_string(d);
    }

    std::string operator() (const std::vector<unsigned char>& vector) const {
        std::stringstream out;
        boost::archive::text_oarchive out_archive(out);
        out_archive << vector;
        return out.str();
    }

    std::string operator() (const std::string &str) const {
        return str;
    }

    std::string operator() (const std::map<std::string, boost::variant<std::string, std::vector<unsigned char>>>& map) const {
        std::stringstream out;

        boost::archive::text_oarchive out_archive(out);
        for (const auto& pair : map) {
            out_archive << boost::apply_visitor(EventToSimVisitor(), pair.second);
        }
        return out.str();
    }
};
}

#endif //SIM_TO_DUT_INTERFACE_EVENTTOSIMVISITOR_H
