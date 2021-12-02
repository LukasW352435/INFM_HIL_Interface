/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      SuzukiCodec.h
 \brief
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/

#ifndef SIM_TO_DUT_INTERFACE_SUZUKICODEC_H
#define SIM_TO_DUT_INTERFACE_SUZUKICODEC_H

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <CANConnectorCodec.h>

namespace sim_interface::dut_connector::can {

    class SuzukiCodec : public CANConnectorCodec {
    public:
        SuzukiCodec() {
            DuTLogger::logMessage("Build Suzuki Codec", LOG_LEVEL::DEBUG, false);
        }

        canfd_frame parseEventToFrame(const SimEvent &event) override;

    };
}

#endif //SIM_TO_DUT_INTERFACE_SUZUKICODEC_H
