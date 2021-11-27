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
#include <string>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/

namespace sim_interface::dut_connector::can {

    class CANConnectorConfig : public ConnectorConfig {

    public:
        explicit CANConnectorConfig(std::set<std::string> operations,
                                    std::map<std::string, int> periodicOperations = {},
                                    bool periodicTimerEnabled = false) : ConnectorConfig(std::move(operations),
                                                                                         std::move(periodicOperations),
                                                                                         periodicTimerEnabled) {}

        // Data member
        std::string interfaceName;

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
