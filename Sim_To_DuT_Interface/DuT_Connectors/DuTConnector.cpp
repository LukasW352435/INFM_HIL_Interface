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
 * @author Michael Schmitz
 * @version 1.0
 */

#include "DuTConnector.h"
#include "../Interface_Logger/InterfaceLogger.h"

#include <utility>
#include <boost/asio.hpp>

namespace sim_interface::dut_connector {
    DuTConnector::DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTEventToSim,
                               const sim_interface::dut_connector::ConnectorConfig &config)
            : queueDuTToSim(std::move(queueDuTEventToSim)), processableOperations(config.operations),
              periodicTimerEnabled(config.periodicTimerEnabled) {
        if (periodicTimerEnabled) {
            io = std::make_shared<boost::asio::io_service>();

            for (const auto &operation: config.periodicOperations) {
                enablePeriodicSending(operation.first, operation.second);
            }
            // have one timer (doing nothing) running at any given time to avoid io->run() to return
            aliveTimer = std::make_unique<PeriodicTimer>(io, 1000000000, SimEvent(), [](const SimEvent &event) {});
            aliveTimer->start();
            timerRunner = std::thread([&]() {
                this->io->run();
            });
        }
    }

    DuTConnector::~DuTConnector() {
        aliveTimer->stop();
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
            InterfaceLogger::logMessage("DuTConnector: Handling event " + simEvent.operation, LOG_LEVEL::INFO);
            if (isPeriodicEnabled(simEvent)) {
                InterfaceLogger::logMessage("DuTConnector: Enabling periodic timer for event " + simEvent.operation,
                                            LOG_LEVEL::INFO);
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
        return periodicTimerEnabled && periodicIntervals.find(simEvent.operation) != periodicIntervals.end();
    }

    void DuTConnector::setupTimer(const SimEvent &simEvent) {
        if (periodicTimerEnabled) {
            // stop timer if already running
            if (periodicTimers.find(simEvent.operation) != periodicTimers.end()) {
                periodicTimers.at(simEvent.operation)->stop();
                periodicTimers.erase(simEvent.operation);
            }
            periodicTimers.emplace(simEvent.operation,
                                   std::make_unique<PeriodicTimer>(io, periodicIntervals[simEvent.operation], simEvent,
                                                                   [this](const SimEvent &event) {
                                                                       this->handleEventSingle(event);
                                                                   }));
            periodicTimers[simEvent.operation]->start();
        }
    }

    void DuTConnector::enablePeriodicSending(const std::string &operation, int periodMs) {
        if (periodicTimerEnabled) {
            periodicIntervals.emplace(operation, periodMs);
        }
    }

}
