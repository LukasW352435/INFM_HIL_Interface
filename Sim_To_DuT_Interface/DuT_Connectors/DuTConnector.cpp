//
// Created by Lukas on 19.10.2021.
//

#include "DuTConnector.h"

#include <utility>

namespace sim_interface::dut_connector {
    DuTConnector::DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim,
                               const sim_interface::dut_connector::ConnectorConfig &config)
            : queueDuTToSim(std::move(queueDuTEventToSim)), processableOperations(config.operations) {
    }

    ConnectorInfo DuTConnector::getConnectorInfo() {
        ConnectorInfo info(
                "Test Connector",
                0x01000000,
                "DuTConnector please override!");
        return info;
    }

    void DuTConnector::handleEvent(const SimEvent &simEvent) {
        if (canHandleSimEvent(simEvent)) {
            std::cout << simEvent << std::endl;
            // TODO process event async
        }
    }

    bool DuTConnector::canHandleSimEvent(const SimEvent &simEvent) {
        return processableOperations.find(simEvent.operation) != processableOperations.end();
    }

    void DuTConnector::sendEventToSim(const SimEvent &simEvent) {
        queueDuTToSim->push(simEvent);
    }
}
