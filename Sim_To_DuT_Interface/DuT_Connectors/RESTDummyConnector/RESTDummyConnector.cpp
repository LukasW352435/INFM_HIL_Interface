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

#include <iostream>
#include <utility>
#include "RESTDummyConnector.h"
#include "ReceiveEndpoint.h"

namespace thi::dut_connector::rest_dummy {
    /**
     * Construct a connector to the REST Dummy DuT with given config, init curl lib for use
     *
     * @param queueDuTToSim queue to write received SimEvents to
     * @param config Configuration of connector containing urls, port and (TODO) supported operations
     */
    RESTDummyConnector::RESTDummyConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                                           const RESTConfig &config)
            : DuTConnector(std::move(queueDuTToSim)) {
        sendCallbackDuT = config.baseUrlDuT + "/send";
        readCallbackDuT = config.baseUrlDuT + "/read?clientUrl=";
        port = config.port;
        callbackConnectorQueryParam = config.baseCallbackUrl + ":" + std::to_string(config.port) + "/rest/DuT";
        enableReceiveFromDuT();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlJsonHeader = curl_slist_append(curlJsonHeader, "Content-Type: application/json");
    }

    /**
     * Deconstructur freeing curl resource
     */
    RESTDummyConnector::~RESTDummyConnector() {
        curl_slist_free_all(curlJsonHeader);
        curl_global_cleanup();
    }

    /**
     * Process the given event
     *
     * @param e SimEvent to handle
     */
    void RESTDummyConnector::handleEvent(const SimEvent &e) {
        if (canHandleSimEvent(e)) {
            sendEventToDuT(e);
        }
    }

    /**
     * Return some basic information like name, version and a short description of this connector
     *
     * @return DuTInfo containing information about this DuT connector
     */
    DuTInfo RESTDummyConnector::getDuTInfo() {
        DuTInfo info(
                "REST Dummy Connector",
                0x0000001,
                "Sample dummy connector to mirror messages to a REST API with the ability to view in the browser");
        return info;
    }

    /**
     * Send the given event to the configured REST DuT
     *
     * @param e SimEvent to send
     */
    void RESTDummyConnector::sendEventToDuT(const SimEvent &e) {
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
        if (responseCode != 200) {
            std::cout << "Error on calling interface handler, HTTP-Code: " << responseCode << std::endl;
        }
    }

    /**
     * Enable receiving events from the DuT by sending the HTTP URL to post it to to the DuT
     * Starts the receiving HTTP endpoint and runs it a separate thread
     */
    void RESTDummyConnector::enableReceiveFromDuT() {
        if (!receiveEndpoint) {
            receiveEndpoint = std::make_unique<ReceiveEndpoint>();
        }
        receiveThread = std::thread([this]() {
            receiveEndpoint->startService(this->port,
                                          [this](auto &&PH1) { sendEventToSim(std::forward<decltype(PH1)>(PH1)); });
        });

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
        if (responseCode != 200) {
            std::cout << std::endl << "Error on calling interface handler, HTTP-Code: " << responseCode << std::endl;
        }
    }

    /**
     * Disable receiving events from the DuT by removing the HTTP URL from the DuT
     * Stop the receiving HTTP endpoint
     */
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
        if (responseCode != 200) {
            std::cout << "Error on calling interface handler, HTTP-Code: " << responseCode << std::endl;
        }
    }

    /**
     * Transform given event to json string for DuT
     *
     * @param e SimEvent to transform
     * @return json string with key=operation and status=value
     */
    std::string RESTDummyConnector::EventToRESTMessage(const SimEvent &e) {
        return R"({"key":")" + e.operation + R"(","status":")" + e.value + R"("})";
    }
}
