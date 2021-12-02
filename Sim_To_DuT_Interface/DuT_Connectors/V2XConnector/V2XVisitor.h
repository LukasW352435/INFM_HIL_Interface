//
// Created by michael on 02.12.21.
//

#ifndef SIM_TO_DUT_INTERFACE_V2XVISITOR_H
#define SIM_TO_DUT_INTERFACE_V2XVISITOR_H

#include "boost/variant.hpp"

namespace sim_interface::dut_connector::v2x {
    class V2XVisitor : public boost::static_visitor<std::vector<unsigned char>> {

    public:

        std::vector<unsigned char> operator()(std::vector<unsigned char> val) const {
            return val;
        }

        std::vector<unsigned char> operator()(int i) const {
            return {};
        }

        std::vector<unsigned char> operator()(double d) const {
            return {};
        }

        std::vector<unsigned char> operator()(const std::string &str) const {
            return {};
        }
    };
}


#endif //SIM_TO_DUT_INTERFACE_V2XVISITOR_H
