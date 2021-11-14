/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
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
 * @author Lukas Wagenlehner
 * @version 1.0
 */

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
