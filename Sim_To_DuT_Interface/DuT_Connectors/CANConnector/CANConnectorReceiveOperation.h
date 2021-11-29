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
#include <cstring>
#include <cassert>
#include <linux/can.h>
#include <linux/can/bcm.h>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorReceiveOperation{

    public:
        // Functions members
        /**
         * Constructor
         *
         * @param operation  - The operation name.
         * @param canID      - The CAN ID of the frame.
         * @param hasMask    - Flag if a mask should be used.
         * @param maskLength - The length of the passed mask.
         * @param maskData   - The mask data that should be used. Only required for operations that have a mask.
         */
        CANConnectorReceiveOperation(std::string operation,
                                     bool isCANFD,
                                     bool hasMask,
                                     int maskLength = 0,
                                     __u8* maskData = nullptr);

        // Data members
        std::string operation;            /**< The operation name.            */
        bool isCANFD;                     /**< Flag for CANFD frames.         */
        bool hasMask;                     /**< Flag if a mask should be used. */
        int maskLength;                   /**< The length of the mask data.   */
        struct canfd_frame mask = {0};    /**< The mask that should be used.  */

    private:
        // Functions members

        // Data members

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/