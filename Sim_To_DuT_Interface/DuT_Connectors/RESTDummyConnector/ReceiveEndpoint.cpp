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

#include <corvusoft/restbed/settings.hpp>
#include <corvusoft/restbed/session.hpp>
#include <corvusoft/restbed/request.hpp>
#include <iostream>
#include <utility>
#include "ReceiveEndpoint.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace thi::dut_connector::rest_dummy {
    /**
     * Constructs the restbed service and runs it on given port
     *
     * @param port Port to start webservice to receive DuT messages on
     * @param function callback to call with received SimEvent
     */
    void ReceiveEndpoint::startService(int const &port, std::function<void(const SimEvent &)> function) {
        if (!m_spReceiveApiResource)
            m_spReceiveApiResource = std::make_shared<ReceiveApiResource>(std::move(function));
        this->publish(m_spReceiveApiResource);

        std::shared_ptr<restbed::Settings> settings = std::make_shared<restbed::Settings>();
        settings->set_port(port);
        settings->set_root("");

        this->start(settings);
    }

    /**
     * Stop webservice
     */
    void ReceiveEndpoint::stopService() {
        this->stop();
    }

    /**
     * Create endpoint for REST service on path /rest/DuT and method PUT
     *
     * @param function callback to call with received SimEvent
     */
    ReceiveApiResource::ReceiveApiResource(std::function<void(const SimEvent &)> function) : eventToSimCallback(
            std::move(function)) {
        this->set_path("/rest/DuT");
        this->set_method_handler("PUT", [this](auto &&PH1) { PUT_method_handler(std::forward<decltype(PH1)>(PH1)); });
    }

    /**
     * Parse body and extract SimEvent and provide it to callback
     *
     * @param session HTTP session on which the request was received
     */
    void ReceiveApiResource::PUT_method_handler(const std::shared_ptr<restbed::Session> &session) {

        const auto request = session->get_request();

        int content_length = request->get_header("Content-Length", 0);
        session->fetch(content_length,
                       [this](const std::shared_ptr<restbed::Session> &session, const restbed::Bytes &body) {
                           const auto request = session->get_request();
                           std::string json = restbed::String::format("%.*s\n", (int) body.size(), body.data());

                           eventToSimCallback(JsonToSimEvent(json));

                           session->close(200, "Event received", {{"Connection", "close"}});
                       });
    }

    /**
     * Extract the SimEvent from given json
     *
     * @param json JSON encoded string
     * @return SimEvent based on json
     */
    SimEvent ReceiveApiResource::JsonToSimEvent(const std::string &json) {
        std::stringstream ss(json);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);
        std::string operation = pt.get("key", "");
        std::string value = pt.get("status", "");
        return {operation, value, "REST_Dummy_DuT"};
    }

}
