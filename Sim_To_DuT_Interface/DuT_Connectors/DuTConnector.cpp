//
// Created by Lukas on 19.10.2021.
//

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
