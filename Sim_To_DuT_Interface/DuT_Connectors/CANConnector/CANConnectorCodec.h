/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorCodec.h
 \brief     The CAN Connector Codec defines the interface that each codec
            implementation must fulfill.
 \author    Matthias Bank
 \version   1.0.0
 \date      16.12.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "../../Events/SimEvent.h"
#include "DuTLogger.h"

// System includes
#include <vector>
#include <linux/can.h>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorCodec{

    public:
        // Function members

        /**
        * Converts an simulation event to a CAN/CANFD payload and determines the sendOperation name.
        *
        * @param event - The simulation event we want to transform into a CAN/CANFD frame payload.
        *
        * @return The CAN/CANFD frame payload.
        */
        virtual std::pair<std::vector<__u8>, std::string> convertSimEventToFrame(SimEvent event) = 0;

        /**
         * Converts a CAN/CANFD frame to the corresponding simulation events.
         *
         * @param frame   - The frame that we want to transform.
         * @param isCanfd - Flag for CANFD frames.
         *
         * @return The simulation events that were contained in the CAN/CANFD frame.
         */
        virtual std::vector<SimEvent> convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd) = 0;

        // Data members

    private:
        // Function members

        // Data members

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/