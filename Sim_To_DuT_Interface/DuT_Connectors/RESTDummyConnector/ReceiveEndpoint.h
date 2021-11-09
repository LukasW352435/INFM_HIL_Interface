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

#ifndef SIM_TO_DUT_INTERFACE_RECEIVEENDPOINT_H
#define SIM_TO_DUT_INTERFACE_RECEIVEENDPOINT_H


#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <memory>
#include "../../Events/SimEvent.h"

namespace thi::dut_connector::rest_dummy {
    class ReceiveApiResource : public restbed::Resource {
    public:
        explicit ReceiveApiResource(std::function<void(const SimEvent &)> function);

        ~ReceiveApiResource() override;

        void PUT_method_handler(const std::shared_ptr<restbed::Session> &session);

    private:
        static SimEvent JsonToSimEvent(const std::string &json);

        std::function<void(const SimEvent &)> eventToSimCallback;
    };

    class ReceiveEndpoint : public restbed::Service {
    public:
        void startService(int const &port, std::function<void(const SimEvent &)> function);

        void stopService();

        std::shared_ptr<ReceiveApiResource> m_spReceiveApiResource;
    };
}

#endif //SIM_TO_DUT_INTERFACE_RECEIVEENDPOINT_H
