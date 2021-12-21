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
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
#define SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H

#include "../Events/SimEvent.h"
#include "../SystemConfig.h"
#include "../SimToDuTInterface.h"
#include <zmq.hpp>

namespace sim_interface {
    // Forward declaration to avoid looping
    class SimToDuTInterface;

    /**
     * <summary>
     * Handler between interface and simulation.
     * Responsible for sending/receiving SimEvents to/from the simulation.
     * </summary>
     */
    class SimComHandler {
    public:

        /**
     	* Constructor: Create a new handler.
     	* @param interface A pointer to communicate with the interface.
     	* @param config System configuration.
     	* socketSimPub_: Create socket for publishing messages to simulation
     	* socketSimSub_: Create socket for subscribing messages from simulation
     	* socketSimSubConfig_: Create socket for subscribing  xml-configuration from simulation
    	*
     	* Configure sockets with the addresses from the config
     	* context_recConfig: Defines the context of the zmq socket for the xml-Configuration
     	* Set the sockets to subscriber sockets and listen to any String
     	*
     	* Open connecting of the sockets:
        * Start Logger for connecting the sockets
     	* Bind: Publisher sockets have to bind to the address
     	* Connect: Subscriber sockets have to connect
     	*/

        SimComHandler(SimToDuTInterface *interface, const SystemConfig &config);

        /**
         * Unbinds, disconnects and closes the sockets
         * Stops all threads and destroys the handler
         */
        ~SimComHandler();

        /**
        * Asynchronous sending of events to simulation.
        * @param simEvent: An event that should be sent to the simulation.
        * logSimEvent: For Logging the SimEvents
        * simEventMap: A variable Map for events to the Simulation
        * adding keys and values to the simEventMap
        * simEventsStringStream: output stringstream for publishing
        * archiveSimEvent: put the stringstream into the output text archive
        * write the Map to the archive
        * simEventData: Convert simEventsStringStream into a string
        * msgToSend: preparing zmq:message type for publishing
        * Send message via socket and start logger for event
    	*/
        void sendEventToSim(const SimEvent &simEvent);

        /**
         * Starts the handler to asynchronously receive incoming events.
         * while loop: ends, if the thread was stopped
         * replySimData: zmq message, which contains serialized data from the simulation
         * Receive the serialized simulation data e.g. Speed with exception-handling
         * If receiving is failed, unbind and disconnect the sockets
         * Start logging
         * bufSimData: Buffer with the serialized string
         * inputSimData: Convert the buffer into a string
         * archiveStreamSimData: for writing the inputSimData
         * archiveSimData: input text archive for writing the inputSimData stringstream
         * receiveMapSimData: variable map with operations and value e.g. speed 13.05
         * writing the archive into the map with exception handling
         * start logging, if writing failed
         * for each loop: run through element from the receiveMapSimData
         * create for each key and value an event
         * Send the Simulation Event to the Interface
         */
        void receive();

        /**
      	* replyConfig: zmq message, which contains serialized data from the xml-config
      	* Receive the serialized data from the xml-config with exception-handling
    	* If receiving is failed, unbind and disconnect the sockets
        * Start logging
     	* bufConfig: Buffer with the serialized string from the xml-config
     	* tree: Property tree for deserialize the xml-config
     	* convertedSimConfigBuffer: output stringstream for converting the buffer
     	* SimConfigString: convert the output stringstream to a string
     	* With these parameters parsing the XML into the property tree.
     	* If parsing is failed, unbind and disconnect the sockets
        * Start logging
        *
     	* connectorTypes: create String for copmaring the names of the connectortypes
     	* xmlWriterSettings: preparing an xml writer with property tree
        *
        * for each loop: run through each value type in a xml-file and get every value in connectors tags
        * save the classtype of the xml attribute into the connectorTypes string for comparing
        * compare the connector types with the enumeration
        *
        * The principle of the deserializing for the three connector are working very similar
        * Exception handling and logging
        * The principle will be explained with RESTDummyConnector:
        * restXMLStringStream: stringstream for writing the xml file
        * writing the xml file with these parameters
        * restXMLString: Convert the stringstream to a string
        * replace not needed tags in the xml-file to make it shorter
        * restConnectorConfig: Create Pointer from type RESTConnectorConfig
        * xmliStringStream: Convert the string to a input stringstream for deserialization
        * Deserialize the received RESTDummyConnector from xml-config
        * Create RestConnector object and put it in the queue of the DuTToSim
        * Add it to the interface
        *
        * Start logging: shows message which connectors were created
        */
        void getConfig();

        /**
        * connectorType: numeration for the various connectorType
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
        * @param connectorTypeS: String for comparing the name of the connector types in the config.xml
        */
        connectorType resolveConnectorTypeForSwitch(std::string connectorTypeS);

        /**
       * starts the receive method in a own thread
       */
        void run();

        /**
        * unbind the publisher socket --> to simulation
        */
        void unbindPublisher();

        /**
        * disconnect the subscriber socket --> from simulation
        */
        void disconnectSubscriber();

        /**
        * disconnect the subscriber config socket --> from simulation
        */
        void disconnectReceiveConfig();

        /**
        * close all sockets
        */
        void close();


    private:
        /**
       * send an event to the interface
       * start logger on the current Simulation Event
       * @param simEvent: Pointer for SimulationEvent e.g. Speed
       */
        void sendEventToInterface(const SimEvent &simEvent);

        zmq::socket_t socketSimSub_; /**< Socket for Interface subscriber */
        zmq::socket_t socketSimPub_; /**< Socket for Interface publisher */
        zmq::socket_t socketSimSubConfig_; /**< Socket for Interface subscriber for the configuration */
        std::string socketSimAddressSub; /**< String for the address socket for subscriber from simulation. */
        std::string socketSimAddressPub; /**< String for the address socket for publisher to simulation. */
        std::string socketSimAddressReceiverConfig; /**< String for the address socket for subscriber for xml-Configuration from simulation. */
        std::thread simComHandlerThread; /**< Thread for receiving simulation data. */
        bool stopThread = true; /**< boolean for starting/stopping the thread. */
        SimToDuTInterface *interface; /**< Pointer form object SimToDuTInterface for adding connectors. */
    };
}

#endif //SIM_TO_DUT_INTERFACE_SIMCOMHANDLER_H
