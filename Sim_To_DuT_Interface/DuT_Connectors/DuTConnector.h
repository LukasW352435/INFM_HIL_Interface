//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H

#include <string>
#include <iostream>
#include <set>
#include <memory>
#include <boost/asio.hpp>
#include "../DuT_Connectors/ConnectorInfo.h"
#include "../Events/SimEvent.h"
#include "../Utility/SharedQueue.h"
#include "../Utility/PeriodicTimer.h"
#include "ConnectorConfig.h"

namespace sim_interface::dut_connector {
    class DuTConnector {
    public:
        explicit DuTConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                              const sim_interface::dut_connector::ConnectorConfig &config);

        ~DuTConnector();

        // return some basic information from the connector
        virtual ConnectorInfo getConnectorInfo();

        // handle event from the simulation async
        void handleEvent(const SimEvent &simEvent);

        // send the event to the simulation
        void sendEventToSim(const SimEvent &simEvent);

    protected:
        virtual void handleEventSingle(const SimEvent &simEvent) {};

    private:

        // determine if an event needs to be processed
        bool canHandleSimEvent(const SimEvent &simEvent);

        bool isPeriodicEnabled(const SimEvent &simEvent);

        void setupTimer(const SimEvent &simEvent);

        void enablePeriodicSending(const std::string& operation, int periodMs);

        std::shared_ptr<boost::asio::io_service> io;
        std::thread timerRunner;
;

        std::set<std::string> processableOperations;

        std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim;

        std::map<std::string, PeriodicTimer> periodicTimers;
        std::map<std::string, int> periodicIntervals;
    };
}

#endif //SIM_TO_DUT_INTERFACE_DUTCONNECTOR_H
