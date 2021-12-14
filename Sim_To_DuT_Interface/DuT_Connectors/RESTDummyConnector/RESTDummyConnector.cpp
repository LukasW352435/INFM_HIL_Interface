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

#include <iostream>
#include <utility>
#include <boost/lexical_cast.hpp>
#include "../../Events/EventVisitor.h"
#include "RESTDummyConnector.h"
#include "ReceiveEndpoint.h"
#include "../../DuTLogger/DuTLogger.h"

namespace sim_interface::dut_connector::rest_dummy {
    RESTDummyConnector::RESTDummyConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                                           const RESTConnectorConfig &config)
            : DuTConnector(std::move(queueDuTToSim), config) {
        sendCallbackDuT = config.baseUrlDuT + "/send";
        readCallbackDuT = config.baseUrlDuT + "/read?clientUrl=";
        port = config.port;
        callbackConnectorQueryParam = config.baseCallbackUrl + ":" + std::to_string(config.port) + "/rest/DuT";
        enableReceiveFromDuT();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlJsonHeader = curl_slist_append(curlJsonHeader, "Content-Type: application/json");
    }

    RESTDummyConnector::~RESTDummyConnector() {
        curl_slist_free_all(curlJsonHeader);
        curl_global_cleanup();
        receiveEndpoint->stopService();
        receiveThread.join();
    }

    ConnectorInfo RESTDummyConnector::getConnectorInfo() {
        ConnectorInfo info(
                "REST Dummy Connector",
                0x0000001,
                "Sample dummy connector to mirror messages to a REST API with the ability to view in the browser");
        return info;
    }

    void RESTDummyConnector::handleEventSingle(const SimEvent &e) {
        auto handle = curl_easy_init();

        long responseCode = 500;
        if (handle) {
            curl_easy_setopt(handle, CURLOPT_URL, sendCallbackDuT.data());
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "RESTDummyConnector");
            auto payload = EventToRESTMessage(e);
            std::cout << payload << std::endl;
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, payload.data());
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curlJsonHeader);
            curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5);

            curl_easy_perform(handle);
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responseCode);
            curl_easy_cleanup(handle);
        }
        if (responseCode == 200) {
            DuTLogger::logMessage("RESTDummyConnector: Successfully sent event " + e.operation + " to DuT", LOG_LEVEL::DEBUG);
        } else {
            DuTLogger::logMessage("RESTDummyConnector: Error on sending event, HTTP-Code: " + std::to_string(responseCode), LOG_LEVEL::ERROR);
        }
    }

    void RESTDummyConnector::enableReceiveFromDuT() {
        if (!receiveEndpoint) {
            receiveEndpoint = std::make_unique<ReceiveEndpoint>();
        }
        receiveThread = std::thread([this]() {
            receiveEndpoint->startService(this->port,
                                          [this](auto &&PH1) { sendEventToSim(std::forward<decltype(PH1)>(PH1)); });
        });
        DuTLogger::logMessage("RESTDummyConnector: Receive endpoint started", LOG_LEVEL::DEBUG);

        auto handle = curl_easy_init();

        long responseCode = 500;
        if (handle) {
            std::string queryParam = curl_easy_escape(handle, callbackConnectorQueryParam.data(), 0);
            std::string url = readCallbackDuT + queryParam;
            curl_easy_setopt(handle, CURLOPT_URL, url.data());
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "RESTDummyConnector");
            curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5);

            curl_easy_perform(handle);
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responseCode);
            curl_easy_cleanup(handle);
        }
        if (responseCode == 200) {
            DuTLogger::logMessage("RESTDummyConnector: Receiving of events enabled", LOG_LEVEL::INFO);
        } else if (responseCode == 208) {
            DuTLogger::logMessage("RESTDummyConnector: Receiving of events was already enabled", LOG_LEVEL::INFO);
        } else {
            DuTLogger::logMessage("RESTDummyConnector: Error on enabling receive, HTTP-Code: " + std::to_string(responseCode), LOG_LEVEL::ERROR);
        }
    }

    void RESTDummyConnector::disableReceiveFromDuT() {
        receiveEndpoint->stopService();
        auto handle = curl_easy_init();

        long responseCode = 500;
        if (handle) {
            std::string queryParam = curl_easy_escape(handle, callbackConnectorQueryParam.data(), 0);
            curl_easy_setopt(handle, CURLOPT_URL, (readCallbackDuT + queryParam).data());
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "RESTDummyConnector");
            curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5);

            curl_easy_perform(handle);
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responseCode);
            curl_easy_cleanup(handle);
        }
        if (responseCode == 200) {
            DuTLogger::logMessage("RESTDummyConnector: Receiving of events disabled", LOG_LEVEL::INFO);
        } else {
            DuTLogger::logMessage("RESTDummyConnector: Error on disabling receive, HTTP-Code: " + std::to_string(responseCode), LOG_LEVEL::ERROR);
        }
    }

    std::string RESTDummyConnector::EventToRESTMessage(const SimEvent &e) {
        return R"({"key":")" + e.operation + R"(","status":")" + boost::apply_visitor(EventVisitor(), e.value) + R"("})";
    }
}
