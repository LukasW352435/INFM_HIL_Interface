//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H

#include <string>
#include <iostream>
#include <set>
#include <memory>
#include "../DuT_Connectors/ConnectorInfo.h"
#include "../Events/SimEvent.h"
#include "../Utility/SharedQueue.h"
#include "ConnectorConfig.h"

namespace sim_interface::dut_connector {
    class DuTConnector {
    public:
        explicit DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                              const sim_interface::dut_connector::ConnectorConfig &config);

        // return some basic information from the connector
        virtual ConnectorInfo getConnectorInfo();

        // handle event from the simulation async
        virtual void handleEvent(const SimEvent &simEvent);

// send the event to the simulation
        void sendEventToSim(const SimEvent &simEvent);

    protected:
        // determine if an event needs to be processed
        bool canHandleSimEvent(const SimEvent &simEvent);

    private:
        std::set<std::string> processableOperations;

        std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim;
    };
}

#endif //SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
