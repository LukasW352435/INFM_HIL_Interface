//
// Created by laki on 26.11.21.
//

#ifndef SIM_TO_DUT_INTERFACE_SYSTEMCONFIG_H
#define SIM_TO_DUT_INTERFACE_SYSTEMCONFIG_H

namespace sim_interface {
    class SystemConfig{
    public:
        std::string socketSimAddressSub = "tcp://localhost:7777";
        std::string socketSimAddressPub = "tcp://*:7778";
    };
}

#endif //SIM_TO_DUT_INTERFACE_SYSTEMCONFIG_H
