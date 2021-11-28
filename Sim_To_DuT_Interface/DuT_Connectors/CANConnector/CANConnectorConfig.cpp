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
                                           std::map<std::string, int> periodicOperations = {},
                                           bool periodicTimerEnabled = false,
                                           std::vector<CANConnectorReceiveOperation> recvOperations = {},
                                           std::vector<CANConnectorSendOperation> sendOperations = {}):
                                           ConnectorConfig(operations,periodicOperations, periodicTimerEnabled),
                                           interfaceName(std::move(interfaceName)){



    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/