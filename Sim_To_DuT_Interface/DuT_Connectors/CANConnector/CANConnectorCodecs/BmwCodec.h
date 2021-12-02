/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.h
 \brief
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/

#ifndef SIM_TO_DUT_INTERFACE_BMWCODEC_H
#define SIM_TO_DUT_INTERFACE_BMWCODEC_H

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <CANConnectorCodec.h>

namespace sim_interface::dut_connector::can {

    class BmwCodec : public CANConnectorCodec {
    public:
        BmwCodec() {
            DuTLogger::logMessage("Build Bmw Codec", LOG_LEVEL::DEBUG, false);
        }

        canfd_frame parseEventToFrame(const SimEvent &event) override;

    };
}

#endif //SIM_TO_DUT_INTERFACE_BMWCODEC_H
