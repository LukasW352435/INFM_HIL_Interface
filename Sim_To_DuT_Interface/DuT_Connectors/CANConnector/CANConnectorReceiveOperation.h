/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorReceiveOperation.h
 \brief     Contains the necessary information to configure a receive operation.
            Used for the configuration of the CAN Connector.
 \author    Matthias Bank
 \version   1.0.0
 \date      27.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes

// System includes
#include <string>
#include <linux/can.h>
#include <linux/can/bcm.h>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorReceiveOperation{

    public:
        // Functions members

        // Data members

    private:
        // Functions members

        // Data members
        std::string operation;      /**< The operation name.            */
        canid_t canID;              /**< The CAN ID of the frame.       */
        bool isCANFD;               /**< Flag for CANFD frames.         */
        bool hasMask;               /**< Flag if a mask should be used. */
        __u8 mask[CAN_MAX_DLEN];    /**< The mask that should be used.  */

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/