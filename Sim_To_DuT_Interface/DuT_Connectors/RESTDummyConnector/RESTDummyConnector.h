/**
 * HIL - REST Dummy Connector
 * This the connector to interface with the REST Dummy DuT
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - REST Dummy Connector".
 *
 * "HIL - REST Dummy Connector" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - REST Dummy Connector" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - REST Dummy Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_RESTDUMMYCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_RESTDUMMYCONNECTOR_H

#include "../DuTConnector.h"
#include "RESTConnectorConfig.h"
#include "ReceiveEndpoint.h"
#include <curl/curl.h>
#include <thread>

namespace sim_interface::dut_connector::rest_dummy {
    class RESTDummyConnector : public DuTConnector {

    public:
        /**
         * Construct a connector to the REST Dummy DuT with given config, init curl lib for use
         *
         * @param queueDuTToSim queue to write received SimEvents to
         * @param config Configuration of connector containing urls, port and supported operations
         */
        explicit RESTDummyConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim, const RESTConnectorConfig &config);

        /**
         * Deconstructor freeing curl resource
         */
        ~RESTDummyConnector();

        /**
         * Return some basic information like name, version and a short description of this connector
         *
         * @return ConnectorInfo containing information about this DuT connector
         */
        ConnectorInfo getConnectorInfo() override;

        /**
         * Send the given event to the configured REST DuT
         *
         * @param e SimEvent to send
         */
        void handleEventSingle(const SimEvent &e) override;

    private:
        std::unique_ptr<ReceiveEndpoint> receiveEndpoint;
        std::string sendCallbackDuT;
        std::string readCallbackDuT;
        std::string callbackConnectorQueryParam;
        int port;
        std::thread receiveThread;
        struct curl_slist *curlJsonHeader = nullptr;

        /**
         * Transform given event to json string for DuT
         *
         * @param e SimEvent to transform
         * @return json string with key=operation and status=value
         */
        static std::string EventToRESTMessage(const SimEvent &e);

        /**
         * Enable receiving events from the DuT by sending the HTTP URL to post it to to the DuT
         * Starts the receiving HTTP endpoint and runs it a separate thread
         */
        void enableReceiveFromDuT();

        /**
         * Disable receiving events from the DuT by removing the HTTP URL from the DuT
         * Stop the receiving HTTP endpoint
         */
        void disableReceiveFromDuT();
    };
}

#endif //SIM_TO_DUT_INTERFACE_RESTDUMMYCONNECTOR_H
