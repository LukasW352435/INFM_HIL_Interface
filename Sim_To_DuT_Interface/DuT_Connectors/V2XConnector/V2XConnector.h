/**
 * HIL - V2X Connector
 * This the connector to interface with the V2X DuT
 *
 * Copyright (C) 2021 Franziska Ihrler
 *
 * This file is part of "HIL - V2X Connector".
 *
 * "HIL - V2X Connector"" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - V2X Connector"" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - REST Dummy Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Franziska Ihrler
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H


#include "../DuTConnector.h"
#include <curl/curl.h>
#include <thread>
#include "V2XConfig.h"

#include "cube-its/catkin_ws/src/vanetza/src/vanetza_ros_node.h"
#include <vanetza/btp/data_indication.hpp>
#include <vanetza/geonet/transport_interface.hpp>



namespace sim_interface::dut_connector::v2x{
    class V2XConnector : public DuTConnector {

    public:
        explicit V2XConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim, const V2XConfig &config);

        ~V2XConnector();

        void handleEvent(const SimEvent &e) override;

        ConnectorInfo getConnectorInfo() override;
        void sendEventToDuT(const SimEvent &e);
        void sendV2XEventToSim(const vanetza::geonet::MIB mib);

    private:

        VanetzaROSNode vanetza_ros_node;
        // std::unique_ptr<ReceiveEndpoint> receiveEndpoint;
        ContextParams params;
        std::thread receiveThread;

        void enableReceiveFromDuT();



    };
}



#endif //SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H
