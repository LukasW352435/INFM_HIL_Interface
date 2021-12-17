/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
 *
 * This file is part of "Sim To DuT Interface".
 *
 * "Sim To DuT Interface" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sim To DuT Interface" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Lukas Wagenlehner
 * @author Fabian Andre Genes
 * @author Thanaancheyan Thavapalan
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
#define SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H

/**
 * Included Headerfiles
 *
 * 1) Created Headerfiles
 *      - ConfigSerializer: Functions for de-/serialization for the various connectorconfigs
 *      - ConnectorConfig: Header from ConnectorConfig
 *      - RESTConnectorConfig: Header from RESTConnectorConfig
 *      - CANConnectorConfig: Header from CANConnectorConfig
 *      - SimEvent: Header from SimEvent
 *      - SharedQueue: Header from SharedQueue
 *      - SystemConfig: Header from SystemConfig
 *
 * 2)  Standard functions
 *      - memory
 *      - string
 *      - thread
 *      - utility
 *      - vector
 *
 * 3)   Zmq: Libary for creating Sockets and receive/ send String messages to other Sockets
 *
 * 4)   Boost:
 *      - archive: in-/output archive for text, binary and xml for reading and writing files
 *      - property_tree: for reading a xml-file correctly
 *      - seriallization: for de-/serialization various data structs
 *
**/


#include "../Utility/ConfigSerializer.h"
#include "../DuT_Connectors/ConnectorConfig.h"
#include "../DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"
#include "../DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "../Events/SimEvent.h"
#include "../Utility/SharedQueue.h"
#include "../SystemConfig.h"


#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <zmq.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>

#include <boost/algorithm/string.hpp>


namespace sim_interface {
    /**
     * Handler between interface and simulation.
     * Responsible for sending/receiving SimEvents to/from the simulation.
     */
    class SimComHandler {
    public:
        /**
         * Create a new handler.
         * @param queueSimToInterface A queue to communicate with the interface.
         * @param config System configuration.
         */
        SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, const SystemConfig& config);

        /**
         * Destroys the handler and stops all threads.
         */
        ~SimComHandler();

        /**
         * Asynchronous sending of events to simulation.
         * @param simEvent An event that should be sent to the simulation.
         */
        void sendEventToSim(const SimEvent &simEvent);

        /**
         * Starts the handler to asynchronously receive incoming events.
         */
        void run();

          /**
         * Get the various ConnectorTypes from the config.xml
         * @param *RESTConnectorVektor: Standardvector from Type RESTConnectorConfig Pointer
         * @param *CanConnectorVektor: Standardvector from Type CanConnectorConfig Pointer
         */
        void getConfig(std::vector<sim_interface::dut_connector::rest_dummy::RESTConnectorConfig*> *RESTConnectorVektor,
                       std::vector<sim_interface::dut_connector::can::CANConnectorConfig*> *CanConnectorVektor);

        /**
         * Get the various ConnectorTypes from the config.xml
         * @param *connectorConfig: Standardvector from Type ConnectorConfig Pointer
         * Problem: The Standardvector has only objects from type ConnectorConfig and not form the specifiic objectts e.g. RESTConnectorConfig
         */

        void getConfig(  std::vector<sim_interface::dut_connector::ConnectorConfig*> *connectorConfig);

        /**
         * @param *connectorType: numeration for the various connectorType
         * RESTDummyConnector
         * CANConnector
         * V2XConnector
         * Invalid_Connector: If the configuration of a connector is invalid in the config.xml
         */
        enum connectorType {
            RESTDummyConnector,
            CANConnector,
            V2XConnector,
            Invalid_Connector
        };

        /**
        * function for checking the various connector types in the config file
        * @param *connectorTypeS: String for comparing the name of the connector types in the config.xml
        */
        connectorType resolveConnectorTypeForSwitch(std::string connectorTypeS);



    private:
        /**
        * send an event to the interface
        * @param *simEvent: Pointer for SimulationEvent e.g. Speed
        */
        void sendEventToInterface(const SimEvent &simEvent);

        /**
        * Create zmq sockets for Publisher and Subscrribe
        * @param *socketSimSub: Socket for Interface subscriber
        * @param *socketSimPub: Socket for Interface publisher
        * @param *socketSimSubConfig_: Socket for Interface subscriber for the configuration
        */
        zmq::socket_t socketSimSub_;
        zmq::socket_t socketSimPub_;
        zmq::socket_t socketSimSubConfig_;

        /**
        * @param *queueSimToInterface: used by sendEventToInterface
        */
        std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface;
    };
}

#endif //SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
