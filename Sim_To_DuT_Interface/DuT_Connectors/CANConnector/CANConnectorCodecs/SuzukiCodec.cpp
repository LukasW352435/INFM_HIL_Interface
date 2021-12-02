/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      SuzukiCodec.cpp
 \brief     Implements the CAN codec for the Suzuki.
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "SuzukiCodec.h"

// System includes


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    canfd_frame SuzukiCodec::parseEventToFrame(const SimEvent &event){
        DuTLogger::logMessage("Suzuki Codec received event", LOG_LEVEL::DEBUG);
        std::cout << "ParseEventToFrame Suzuki" << std::endl;
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/