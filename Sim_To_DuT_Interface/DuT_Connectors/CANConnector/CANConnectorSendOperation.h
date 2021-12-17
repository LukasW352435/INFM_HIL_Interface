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
#include <stdexcept>
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
         * @param canID     - The CAN ID of the frame.
         * @param isCANFD   - Flag for CANFD frames.
         * @param isCyclic  - Flag for cyclic frames.
         * @param announce  - Flag for immediately sending out updates once.
         * @param count     - Number of times the frame is send with the first interval.
         * @param ival1     - First Interval.
         * @param ival2     - Second Interval.
         */
        CANConnectorSendOperation(canid_t canID,
                                  bool isCANFD,
                                  bool isCyclic,
                                  bool announce = false,
                                  __u32 count = 0,
                                  struct bcm_timeval ival1 = {0},
                                  struct bcm_timeval ival2 = {0});

        // Data members
        canid_t canID;                  /**< The CAN ID of the frame.                                   */
        bool isCANFD;                   /**< Flag for CANFD frames.                                     */
        bool isCyclic;                  /**< Flag for cyclic messages.                                  */
        bool announce;                  /**< Flag for immediately sending out updates once.             */
        __u32 count;                    /**< Number of times the frame is send with the first interval. */
        struct bcm_timeval ival1;       /**< First Interval.                                            */
        struct bcm_timeval ival2;       /**< Second Interval.                                           */

    private:
        // Functions members

        // Data members

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/