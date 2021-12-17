/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorConfig.h
 \brief     The Config for the CANConnector.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "../ConnectorConfig.h"
#include "CANConnectorReceiveOperation.h"
#include "CANConnectorSendOperation.h"

// System includes
#include <set>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorConfig : public ConnectorConfig{

    public:
        // Function members

        /**
         * Constructor for the CAN Connector configuration.
         *
         * @param interfaceName        - The name of the interface that should be used by the connector.
         * @param operations           - The operations supported by the CAN Connector.
         * @param codecName            - The name of the codec that should be used by the connector.
         * @param frameToOperation     - The configurations for the receive operations.
         * @param operationToFrame     - The configurations for the send operations.
         * @param periodicOperations   - The periodic operations supported by the connector through the interface.
         * @param periodicTimerEnabled - Flag that enables the periodic sending through the interface.
         *                               Since the BCM socket supports cyclic operations this should always be false
         */
        explicit CANConnectorConfig(std::string interfaceName,
                                    std::string codecName,
                                    std::set<std::string> operations,
                                    std::map<canid_t, CANConnectorReceiveOperation> frameToOperation = {},
                                    std::map<std::string, CANConnectorSendOperation> operationToFrame = {},
                                    std::map<std::string, int> periodicOperations = {},
                                    bool periodicTimerEnabled = false);

        // Data member
        std::string interfaceName;                                         /**< The name of the interface that should be used.               */
        std::string codecName;                                             /**< The name of the codec that should be used.                   */

        /**
         * This map is used to set up the RX filters of the BCM socket based on the receive operation data
         * that was defined in the XML configuration file.
         */
        std::map<canid_t, CANConnectorReceiveOperation>  frameToOperation;/

        /**
         * This map is used to get the send operation data (like the CAN ID, interval information etc.) that
         * was defined in the XML configuration file. The Key that is used is returned by the Codec and must
         * therefore match with a sendOperation entry that is defined in the XML configuration file.
         */
        std::map<std::string, CANConnectorSendOperation> operationToFrame; /**< This map is used to*/

    private:
        // Function members

        // Data member

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
