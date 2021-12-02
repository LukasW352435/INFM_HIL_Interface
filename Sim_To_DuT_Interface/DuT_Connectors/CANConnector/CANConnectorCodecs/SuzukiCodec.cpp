/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      SuzukiCodec.cpp
 \brief
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/

#include "SuzukiCodec.h"

namespace sim_interface::dut_connector::can {

    canfd_frame SuzukiCodec::parseEventToFrame(const SimEvent &event) {
        DuTLogger::logMessage("Suzuki Codec received event", LOG_LEVEL::DEBUG);
        std::cout << "ParseEventToFrame Suzuki" << std::endl;
    }
}