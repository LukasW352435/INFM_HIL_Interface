/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorConfig.h
 \brief     The Config for the CANConnector.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "CANConnectorConfig.h"


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    CANConnectorConfig::CANConnectorConfig(std::string interfaceName,
                                           std::set<std::string> operations,
                                           std::map<canid_t, CANConnectorReceiveOperation> frameToOperation = {},
                                           std::map<std::string, CANConnectorSendOperation> operationToFrame = {},
                                           std::map<std::string, int> periodicOperations = {},
                                           bool periodicTimerEnabled = false):
                                           ConnectorConfig(std::move(operations),std::move(periodicOperations), periodicTimerEnabled),
                                           interfaceName(std::move(interfaceName)),
                                           frameToOperation(std::move(frameToOperation)),
                                           operationToFrame(std::move(operationToFrame)){

        // Assert that the interface name is not empty
        assert(!this->interfaceName.empty());

        // Assert that at least one map has an entry
        assert(!this->frameToOperation.empty() || !this->operationToFrame.empty());
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/