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

#include <iostream>
#include <utility>
#include "V2XConnector.h"



namespace sim_interface::dut_connector::v2x {
    /**
     *
     * @param queueDuTToSim queue to write received SimEvents to
     * @param config Configuration of connector containing params (Context Params from Cube Board)
     */
    V2XConnector::V2XConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                               const V2XConfig &config)
            : DuTConnector(std::move(queueDuTToSim), config) {
        params = config.params;

        vanetza_ros_node = VanetzaROSNode();



        std::cout << "Created V2xConnector with multicast-ip: " << params.multicastIp << std::endl;
    }

/**
 * Deconstructur
 */
    V2XConnector::~V2XConnector() {

    }

/**
 * Process the given event
 *
 * @param e SimEvent to handle
 */
    void V2XConnector::handleEvent(const SimEvent &e) {
        //TODO include canHandleSimEvent
            // if (canHandleSimEvent(e)) {}
                 sendEventToDuT(e);

    }

/**
 * Return some basic information like name, version and a short description of this connector
 *
 * @return ConnectorInfo containing information about this DuT connector
 */
    ConnectorInfo V2XConnector::getConnectorInfo() {

        ConnectorInfo info(
                "V2X Connector",
                0x0000002,
                " ");
        return info;

    }

/**
 * Send the given event to the configured V2X DuT
 *
 * @param e SimEvent to send
 */
    void V2XConnector::sendEventToDuT(const SimEvent &e) {
        vanetza::geonet::DataIndication dataIndication;
        std::unique_ptr<vanetza::UpPacket> upPacket;
        //TODO Mapping SimEvent <-> dataIndication, upPacket

        vanetza_ros_node.getBtpPublisher().indicate(dataIndication, upPacket);
    }

    void V2XConnector::enableReceiveFromDuT() {

    }

    void V2XConnector::sendV2XEventToSim(const vanetza::geonet::MIB mib){
    	const SimEvent e;
    	//TODO Mapping mib <-> e
    	this->sendEventSim(e);
    }

}
