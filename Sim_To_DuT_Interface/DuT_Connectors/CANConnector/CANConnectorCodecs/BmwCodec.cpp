/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.cpp
 \brief     Implements the CAN codec for the BMW.
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "BmwCodec.h"

// System includes


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    canfd_frame BmwCodec::parseEventToFrame(const SimEvent &event) {
        DuTLogger::logMessage("BMW Codec received event", LOG_LEVEL::DEBUG);
        std::cout << "ParseEventToFrame BMW " + event.operation << std::endl;
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/