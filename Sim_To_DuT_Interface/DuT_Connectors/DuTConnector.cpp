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
#include <boost/asio.hpp>

namespace sim_interface::dut_connector {
    DuTConnector::DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim,
                               const sim_interface::dut_connector::ConnectorConfig &config)
            : queueDuTToSim(std::move(queueDuTEventToSim)), processableOperations(config.operations) {
        io = std::make_shared<boost::asio::io_service>();
        for (const auto& operation : config.operations) {
            enablePeriodicSending(operation, 100);
        }
        timerRunner = std::thread([this]() {this->io->run();});
    }

    DuTConnector::~DuTConnector() {
        io->stop();
        timerRunner.join();
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
            if(isPeriodicEnabled(simEvent)) {
                setupTimer(simEvent);
            }
            handleEventSingle(simEvent);
        }
    }

    bool DuTConnector::canHandleSimEvent(const SimEvent &simEvent) {
        return processableOperations.find(simEvent.operation) != processableOperations.end();
    }

    void DuTConnector::sendEventToSim(const SimEvent &simEvent) {
        queueDuTToSim->push(simEvent);
    }

    bool DuTConnector::isPeriodicEnabled(const SimEvent &simEvent) {
        return periodicIntervals.find(simEvent.operation) != periodicIntervals.end();
    }

    void DuTConnector::setupTimer(const SimEvent &simEvent) {
        // stop timer if already running
        if (periodicTimers.find(simEvent.operation) != periodicTimers.end()) {
            periodicTimers.at(simEvent.operation).stop();
            periodicTimers.erase(simEvent.operation);
        }
        periodicTimers.emplace(simEvent.operation, PeriodicTimer(io, periodicIntervals[simEvent.operation], simEvent, [this](const SimEvent& event){
                                                                  this->handleEventSingle(event); }));
    }

    void DuTConnector::enablePeriodicSending(const std::string& operation, int periodMs) {
        periodicIntervals.emplace(operation, periodMs);
    }

}
