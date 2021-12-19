/**
 * HIL - V2X Connector
 * This the connector to interface with the V2X DuT
 * IMPORTANT: root privileged required for binding to raw socket
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
 * along with "HIL - V2X Connector".  If not, see <http://www.gnu.org/licenses/>.
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

namespace sim_interface::dut_connector::v2x {
    class V2XConnector : public DuTConnector {

    public:
        /**
         * Initialize V2X Connector and setup socket for receiving ethernet frames
         *
         * @param queueDuTToSim queue to write received SimEvents to
         * @param config Configuration of connector containing params (Context Params from Cube Board)
         */
        explicit V2XConnector(std::shared_ptr<SharedQueue < SimEvent>>

        queueDuTToSim,
        const V2XConnectorConfig &config
        );

        /**
         * Deconstructor stopping ioService and waiting for thread to join
         */
        ~V2XConnector();

        /**
         * Process the given event and send it via the given interface
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
        /**
         * Process the given bytes and enqueue the corresponding SimEvent
         *
         * @param msg bytes received from device
         */
        void receiveCallback(const std::vector<unsigned char> &msg);

        /**
         * Starts a single asynchronous receive operation on receiveEndpoint
         */
        void startReceive();

        /**
         * Handler to call from async receive to handle input of data over device,
         * restart async receive if no error was detected
         * @param ec Error code from async receive
         * @param read_bytes Number of bytes read
         */
        void onReceive(const boost::system::error_code &ec, std::size_t read_bytes);

        /**
         * Get index of given interface
         *
         * @param ifname interface name to get index for
         * @return index of interface
         */
        static int getIfnameIndex(const std::string &ifname);

        /**
         * ioService handle to process async operations
         */
        boost::asio::io_service ioService;

        /**
         * Socket to send and receive over
         */
        boost::asio::generic::raw_protocol::socket _socket;

        /**
         * Buffer to write received bytes into
         */
        std::vector<unsigned char> receiveBuffer;

        /**
         * Endpoint on which the receive operation is performed
         */
        boost::asio::generic::raw_protocol::endpoint receiveEndpoint;

        /**
         * Thread to run ioService in
         */
        std::thread sockRunner;

        /**
         * two bytes to set the ethernet frame type with
         */
        unsigned short ethernetFrameType;
    };
}


#endif //SIM_TO_DUT_INTERFACE_V2XCONNECTOR_H
