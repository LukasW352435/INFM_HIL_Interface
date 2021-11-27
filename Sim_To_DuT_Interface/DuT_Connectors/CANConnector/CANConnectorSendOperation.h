/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorSendOperation.h
 \brief     Contains the necessary information to configure a send operation.
            Used for the configuration of the CAN Connector.
 \author    Matthias Bank
 \version   1.0.0
 \date      27.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H


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

    class CANConnectorSendOperation{

    public:
        // Functions members

        /**
         * Constructor
         *
         * @param operation - The operation name. Always required.
         * @param canID     - The CAN ID of the frame. Always required.
         * @param count     - Number of times the frame is send with the first interval. Only required for cyclic operations.
         * @param ival1     - First Interval. Only required for cyclic operations.
         * @param ival2     - Second Interval. Only required for cyclic operations.
         * @param isCANFD   - Flag for CANFD frames. Always required.
         * @param announce  - Flag for immediately sending out updates once. Only required for cyclic operations.
         * @param isCyclic  - Flag for cyclic messages. Always required.
         */
        CANConnectorSendOperation(std::string operation, canid_t canID, __u32 count, struct bcm_timeval ival1,
                struct bcm_timeval ival2, bool isCANFD, bool announce, bool isCyclic);

        // Data members
        std::string operation;          /**< The operation name.                                        */
        canid_t canID;                  /**< The CAN ID of the frame.                                   */
        __u32 count;                    /**< Number of times the frame is send with the first interval. */
        struct bcm_timeval ival1;       /**< First Interval.                                            */
        struct bcm_timeval ival2;       /**< Second Interval.                                           */
        bool isCANFD;                   /**< Flag for CANFD frames.                                     */
        bool announce;                  /**< Flag for immediately sending out updates once.             */
        bool isCyclic;                  /**< Flag for cyclic messages.                                  */

    private:
        // Functions members

        // Data members

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/