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
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H


#include "../DuTConnector.h"
#include <curl/curl.h>
#include <thread>
#include "V2XConnectorConfig.h"


namespace sim_interface::dut_connector::v2x{
    class V2XConnector : public DuTConnector {

    public:
        /**
         *
         * @param queueDuTToSim queue to write received SimEvents to
         * @param config Configuration of connector containing params (Context Params from Cube Board)
         */
        explicit V2XConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim, const V2XConnectorConfig &config);

        ~V2XConnector();

        /**
         * Process the given event
         *
         * @param e SimEvent to handle
         */
        void handleEventSingle(const SimEvent &e) override;

        /**
         * Return some basic information like name, version and a short description of this connector
         *
         * @return ConnectorInfo containing information about this DuT connector
         */
        ConnectorInfo getConnectorInfo() override;
    private:
        void receiveCallback(const std::vector<unsigned char> &msg);
        void startReceive();
        void onReceive(const boost::system::error_code& ec, std::size_t read_bytes);
        boost::asio::io_service ioService;
        boost::asio::generic::raw_protocol::socket _socket;
        std::vector<unsigned char>receiveBuffer;
        boost::asio::generic::raw_protocol::endpoint receiveEndpoint;
        std::thread sockRunner;

    };
}



#endif //SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H
