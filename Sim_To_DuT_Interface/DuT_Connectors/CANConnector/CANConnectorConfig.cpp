/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorConfig.cpp
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
                                           std::string codecName,
                                           std::set<std::string> operations,
                                           std::map<canid_t, CANConnectorReceiveOperation> frameToOperation,
                                           std::map<std::string, CANConnectorSendOperation> operationToFrame,
                                           std::map<std::string, int> periodicOperations,
                                           bool periodicTimerEnabled):
                                           ConnectorConfig(std::move(operations),"RESTConnectorConfig",std::move(periodicOperations), periodicTimerEnabled),
                                           interfaceName(std::move(interfaceName)),
                                           codecName(std::move(codecName)),
                                           frameToOperation(std::move(frameToOperation)),
                                           operationToFrame(std::move(operationToFrame)){

        // Assert that the interface name is not empty
        if(this->interfaceName.empty()){
            throw std::invalid_argument("CAN Connector Config: The interface name must not be empty in the config");
        }

        // Assert that the codec name is not empty
        if(this->codecName.empty()){
            throw std::invalid_argument("CAN Connector Config: The codec name must not be empty in the config");
        }

        // Assert that at least one map has an entry
        if(this->frameToOperation.empty() && this->operationToFrame.empty()){
            throw std::invalid_argument("CAN Connector Config: There should be at least one sendOperation or receiveOperation in the config");
        }

    }
}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/