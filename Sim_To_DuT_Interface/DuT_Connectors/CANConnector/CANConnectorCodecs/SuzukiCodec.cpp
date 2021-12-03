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

    std::vector<__u8> SuzukiCodec::convertSimEventToFrame(SimEvent event){
        std::vector<__u8> payload;
        return payload;
    }

    std::vector<SimEvent> SuzukiCodec::convertFrameToInt(struct canfd_frame frame, bool isCanfd){
        std::vector<SimEvent> events;

        auto testEvent = sim_interface::SimEvent();
        testEvent.operation = "Geschwindigkeit";
        testEvent.value     = 30;

        events.insert(events.begin(), testEvent);
        return events;
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/