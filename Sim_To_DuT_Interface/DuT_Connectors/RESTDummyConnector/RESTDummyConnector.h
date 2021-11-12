/**
 * HIL - REST Dummy Connector
 * This the connector to interface with the REST Dummy DuT
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - REST Dummy Connector".
 *
 * "HIL - REST Dummy DuT" is free software: you can redistribute it and/or modify
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
#include "RESTConfig.h"
#include "ReceiveEndpoint.h"
#include <curl/curl.h>
#include <thread>

namespace thi::dut_connector::rest_dummy {
    class RESTDummyConnector : public DuTConnector {

    public:
        explicit RESTDummyConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim, const RESTConfig &config);

        ~RESTDummyConnector();

        void handleEvent(const SimEvent &e) override;

        DuTInfo getDuTInfo() override;

    private:
        std::unique_ptr<ReceiveEndpoint> receiveEndpoint;
        std::string sendCallbackDuT;
        std::string readCallbackDuT;
        std::string callbackConnectorQueryParam;
        int port;
        std::thread receiveThread;


        void sendEventToDuT(const SimEvent &e);

        struct curl_slist *curlJsonHeader = nullptr;

        static std::string EventToRESTMessage(const SimEvent &e);

        void enableReceiveFromDuT();

        void disableReceiveFromDuT();
    };
}

#endif //SIM_TO_DUT_INTERFACE_RESTDUMMYCONNECTOR_H
