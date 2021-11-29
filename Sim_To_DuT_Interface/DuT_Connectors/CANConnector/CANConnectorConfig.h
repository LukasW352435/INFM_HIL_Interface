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
#include <cassert>


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
         * @param operations           - The operations supported by the connector.
         * @param periodicOperations   - The periodic operations supported by the connector.
         * @param periodicTimerEnabled - Flag that enables the periodic sending through the interface.
         * @param recvOperations       - The configurations for the receive operations.
         * @param sendOperations       - The configuration for the send operations.
         */
        explicit CANConnectorConfig(std::string interfaceName,
                                    std::set<std::string> operations,
                                    std::map<canid_t, CANConnectorReceiveOperation> frameToOperation,
                                    std::map<std::string, CANConnectorSendOperation> operationToFrame,
                                    std::map<std::string, int> periodicOperations,
                                    bool periodicTimerEnabled);

        // Data member
        std::string interfaceName;                                         /**< The name of the interface that should be used.*/
        std::map<canid_t, CANConnectorReceiveOperation>  frameToOperation; /**< Used for mapping a CAN frame to an operation. */
        std::map<std::string, CANConnectorSendOperation> operationToFrame; /**< Used for mapping an operation to a CAN frame. */

    private:
        // Function members

        // Data member

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
