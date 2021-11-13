//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_CONNECTORINFO_H
#define SIM_TO_DUT_INTERFACE_CONNECTORINFO_H

#include <string>
#include <iostream>

namespace sim_interface::dut_connector {
    class ConnectorInfo {
    public:
        std::string name;
        int version;
        std::string description;

        ConnectorInfo(std::string name, int version, std::string description);

        friend std::ostream &operator<<(std::ostream &os, const ConnectorInfo &duTInfo);
    };
}
#endif //SIM_TO_DUT_INTERFACE_CONNECTORINFO_H
