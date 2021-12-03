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

    BmwCodec::BmwCodec() : hostIsBigEndian(CodecUtilities::checkEndianness()){

        // Test:
        if(hostIsBigEndian) {
            DuTLogger::logMessage("CAN Connector: Host uses big endian", LOG_LEVEL::INFO);
        }else{
            DuTLogger::logMessage("CAN Connector: Host uses little endian", LOG_LEVEL::INFO);
        }

    }

    std::vector<__u8> BmwCodec::convertSimEventToFrame(SimEvent event){
        std::vector<__u8> payload;
        return payload;
    }

    std::vector<SimEvent> BmwCodec::convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd){

        // Vector that contains the resulting simulation events
        std::vector<SimEvent> events;

        switch(frame.can_id){

        }

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