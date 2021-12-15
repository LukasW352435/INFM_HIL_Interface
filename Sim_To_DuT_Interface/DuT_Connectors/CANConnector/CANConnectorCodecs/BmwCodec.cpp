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
            InterfaceLogger::logMessage("CAN Connector: Host uses big endian", LOG_LEVEL::INFO);
        }else{
            InterfaceLogger::logMessage("CAN Connector: Host uses little endian", LOG_LEVEL::INFO);
        }

    }

    std::vector<__u8> BmwCodec::convertSimEventToFrame(SimEvent event){

        std::vector<__u8> payload;

        if(event.operation == "Speed"){
            // Length:  4
            payload.insert(payload.end(), {0xDE, 0xAD, 0xBE, 0xEF});
        }else if(event.operation == "Door"){
            // Length: 12
            payload.insert(payload.end(), {0xC0, 0xFF, 0xEE, 0xC0, 0xFF, 0xEE, 0xC0, 0xFF, 0xEE, 0xC0, 0xFF, 0xEE});
        }else if(event.operation == "Blink"){
            // Length:  4
            payload.insert(payload.end(), {0xBE, 0xEF, 0xDE, 0xAD});
        }else{
            InterfaceLogger::logMessage("CAN Connector: BMW codec received unknown operation: <" + event.operation + ">", LOG_LEVEL::WARNING);
        }

        return payload;
    }

    std::vector<SimEvent> BmwCodec::convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd){

        // Vector that contains the resulting simulation events
        std::vector<SimEvent> events;

        switch(frame.can_id){

            case 0x123:

                events = handleHazardFrame(frame, isCanfd);
                break;

            case 0x456:

                events = handleBrakeFrame(frame, isCanfd);
                break;

            default:

                InterfaceLogger::logMessage("CAN Connector: BMW codec did not implement a conversion for the CAN ID: "
                                      "<" + std::to_string(frame.can_id) + ">", LOG_LEVEL::WARNING);

        }

        return events;
    }

    std::vector<SimEvent> BmwCodec::handleHazardFrame(struct canfd_frame frame, bool isCanfd){

        std::vector<SimEvent> events;

        SimEvent speedEvent;
        speedEvent.operation = "Hazard";
        speedEvent.value     = "Hazard ahead!";
        events.push_back(speedEvent);

        return events;
    }

    std::vector<SimEvent> BmwCodec::handleBrakeFrame(struct canfd_frame frame, bool isCanfd){

        std::vector<SimEvent> events;

        SimEvent brakeEvent;
        brakeEvent.operation = "Brake";
        brakeEvent.value     =  99;
        events.push_back(brakeEvent);

        return events;
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/