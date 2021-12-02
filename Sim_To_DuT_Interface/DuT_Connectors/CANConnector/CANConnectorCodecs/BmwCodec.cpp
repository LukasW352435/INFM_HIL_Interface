/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.cpp
 \brief
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/

#include "BmwCodec.h"

namespace sim_interface::dut_connector::can {

    canfd_frame BmwCodec::parseEventToFrame(const SimEvent &event) {
        DuTLogger::logMessage("BMW Codec received event", LOG_LEVEL::DEBUG);
        std::cout << "ParseEventToFrame BMW " + event.operation << std::endl;
    }
}
