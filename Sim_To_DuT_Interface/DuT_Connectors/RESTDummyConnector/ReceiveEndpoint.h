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

#ifndef SIM_TO_DUT_INTERFACE_RECEIVEENDPOINT_H
#define SIM_TO_DUT_INTERFACE_RECEIVEENDPOINT_H


#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <memory>
#include "../../Events/SimEvent.h"

namespace sim_interface::dut_connector::rest_dummy {
    class ReceiveApiResource : public restbed::Resource {
    public:
        /**
         * Create endpoint for REST service on path /rest/DuT and method PUT
         *
         * @param function callback to call with received SimEvent
         */
        explicit ReceiveApiResource(std::function<void(const SimEvent &)> function);

        /**
         * Parse body and extract SimEvent and provide it to callback
         *
         * @param session HTTP session on which the request was received
         */
        void PUT_method_handler(const std::shared_ptr<restbed::Session> &session);

    private:
        /**
         * Extract the SimEvent from given json
         *
         * @param json JSON encoded string
         * @return SimEvent based on json
         */
        static SimEvent JsonToSimEvent(const std::string &json);

        /**
         * Callback to be called when an event is received from DuT
         */
        std::function<void(const SimEvent &)> eventToSimCallback;
    };

    class ReceiveEndpoint : public restbed::Service {
    public:
        /**
         * Constructs the restbed service and runs it on given port
         *
         * @param port Port to start webservice to receive DuT messages on
         * @param function callback to call with received SimEvent
         */
        void startService(int const &port, std::function<void(const SimEvent &)> function);

        /**
         * Stop webservice
         */
        void stopService();

        /**
         * REST Api resource for receiving and sending events to REST Dummy DuT
         */
        std::shared_ptr<ReceiveApiResource> m_spReceiveApiResource;
    };
}

#endif //SIM_TO_DUT_INTERFACE_RECEIVEENDPOINT_H
