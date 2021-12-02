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
#include <linux/can.h>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorCodec{
    public:
        virtual canfd_frame parseEventToFrame(const SimEvent &event) = 0;

    private:
        void sendEvent(const canfd_frame &frame);
    };
}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCODEC_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/