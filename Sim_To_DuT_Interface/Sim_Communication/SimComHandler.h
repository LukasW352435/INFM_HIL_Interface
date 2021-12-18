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

#include <exception>
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
     	* Constructor: Create a new handler.
     	* @param queueSimToInterface A queue to communicate with the interface.
     	* @param config System configuration.
     	* @param socketSimPub_: Create socket for publisher for messages to simulation
     	* @param socketSimSub_: Create socket for subscriber for messages from simulation
     	* @param socketSimSubConfig_: Create socket for subscriber for xml-Configuration from simulation
    	*
     	* Configure Sockets
     	* @param socketSimAddressPub: String for the address socket for publisher to simulation
     	* @param socketSimAddressSub: String for the address socket for subsriber from simulation
     	* @param socketSimAddressReciverConfig: String for the address socket for subscriber for xml-Configuration from simulation
     	* @param context_recConfig: Defines the context of the zmq socket xml-Configuration
     	*
     	* setsockopt: Set the sockets to subscriber sockets and listen to any String
     	*
     	* Open connecting of the sockets:
     	* Bind: Publisher sockets have to bind to the address
     	* Connect: Subscriber sockets have to connect
     	*/
        SimComHandler(std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface, const SystemConfig &config);

        /**
         * Destroys the handler and stops all threads.
         */
        ~SimComHandler();

        /**
        * Asynchronous sending of events to simulation.
        * @param simEvent An event that should be sent to the simulation.
        * @param simEventMap: A variable Map for events to the Simulation
        * adding keys and values to the simEventMap
        * @param ss: output stringstream for publishing
        * @param archive: put the stringstream into the output text archive
        * write the Map to the archive
        * @param outbound_data: Convert outputstringstream into a string
        * @param msgToSend: preparing zmq:message type for publish
        * Send message via socket
    	*/
        void sendEventToSim(const SimEvent &simEvent);

        /**
        * Starts the handler to asynchronously receive incoming events.
        *
        * @param reply: zmq message_t, which contains serialized data from the xml-config
                * Receive the serialized simulation data with exception-handling e.g. Speed
        * @param *buf: Buffer with the serialized string
        * @param input_data_: Convert the buffer to a String
        * @param *archive_stream: input stringstream for writing the input_data_ in it
        * @param archive: input text archive for writing the input stringstream
        * @param receiveMap: variable map with operations and value e.g. Speed 19,11
        * @param receiveMapDynmaics: variable map with dynamic operations and value e.g. Accelaration 13,05
        * @param connectorTypeS: create String for copmaring the names of the connectortypes
        *
        * writing the archive into the maps with exception handling
        *
        * @param keyVector: Creating vector with keys as Strings
        * @param valueVector: Creating vector with value as variable datatype
        *
        * for each loop: run through element from the receiveMap
        * add the keys to the keyVector; add value to the valueVector
        * --> same Position
        *
        * @param keyAsString: Convert the current key to String
        * @param valueAsAny: Convert the value key to any datatype
        * @param stringStreamValue: write the values into an Stringstream
        * @param event: Create an Simulation Event with these above parameters
        * Send the Simulation Event to the Interface
        *
        *  for each loop: run through element from the receiveMapDynamics
        * add the keys to the keyVector; add value to the valueVector
        * --> same Position
        *
        * @param keyAsString: Convert the current key to String
        * @param valueAsAny: Convert the value key to any datatype
        * @param stringStreamValue: write the values into an Stringstream
        * @param event: Create an Simulation Event with these above parameters
        * Send the Simulation Event to the Interface
        *
        */
        void run();

        /**
      	* Get the various ConnectorTypes from the config.xml
      	* @param *RESTConnectorVektor: Standardvector from Type RESTConnectorConfig Pointer
      	* @param *CanConnectorVektor: Standardvector from Type CanConnectorConfig Pointer
      	* @param *V2XConnectorVektor: Standardvector from Type V2XConnectorConfig Pointer
      	*
      	* @param *reply: zmq message_t, which contains serialized data from the xml-config
      	* Receive the serialized data from the xml-config with exception-handling
    	*
     	* Get the various ConnectorTypes from the config.xml
     	* @param *buf: Buffer with the serialized string
     	* @param *tree: Property tree for deserialize the xml-config
     	* @param *t: output stringstream for writing the buffer in it
     	* @param s: convert the output stringstream to a String
     	* --> with these parameters parsing the XML into the property tree.
     	*
     	* @param connectorTypeS: create String for copmaring the names of the connectortypes
     	*
        *
        * for each loop: run through each value type in a xml-file and get every value in connectors tags
        * save the classtype of the xml attribute into the connector types string for comparing
        * compare the connector types with the enumeration
        *
        * @param xmlWriterSettings: preparing an xml writer with property tree
        * @param xmlStringStream: stringstream for writing the xml file
        * writing the xml file with these parameters
        *
        * @param xmlString: Convert the stringstream to a string
        * replace not needed tags in the xml-file to make it shorter
        * @param *restConnectorConfig: Create Pointer Rest Connector Config
        * @param xmliStringStream: Convert the string to a input stringstream for deserialization
        * Deserialize the received xml-config
        * Serialize the deserialized xml-config for testing
        * add the RestConnectorConfig Pointer to the Vector
        */
        void
        getConfig(std::vector<sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *> *RESTConnectorVektor,
                  std::vector<sim_interface::dut_connector::can::CANConnectorConfig *> *CanConnectorVektor,
                  std::vector<sim_interface::dut_connector::v2x::V2XConnectorConfig *> *V2XConnectorVektor);



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

        void run_thread();


    private:
        /**
       * send an event to the interface
       * start logger on the current Simulation Event
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
        std::thread simComHandlerThread;

        /**
        * @param *queueSimToInterface: used by sendEventToInterface
        */
        std::shared_ptr<SharedQueue<SimEvent>> queueSimToInterface;
    };
}

#endif //SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
