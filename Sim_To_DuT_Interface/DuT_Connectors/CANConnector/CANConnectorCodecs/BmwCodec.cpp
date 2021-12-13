/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.cpp
 \brief     Implements the CAN codec for the BMW.
 \author    Matthias Bank, Marco Keul, Lukas Wagenlehner
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

    BmwCodec::BmwCodec() : hostIsBigEndian(CodecUtilities::checkBigEndianness()){

        // Test:
        if(hostIsBigEndian) {
            DuTLogger::logMessage("CAN Connector: Host uses big endian", LOG_LEVEL::INFO);
        }else{
            DuTLogger::logMessage("CAN Connector: Host uses little endian", LOG_LEVEL::INFO);
        }

    }

    std::vector<__u8> BmwCodec::convertSimEventToFrame(SimEvent event) {

        if(event.operation == "Speed"){
            currentGeschwindigkeitEvents["Speed"] = event;
        }

        if(event.operation == "END_OF_FRAME"){
            // TODO convert boost::variant to uint16_t
            uint16_t speed = (uint16_t)boost::get<double>(currentGeschwindigkeitEvents["Speed"].value);
            encodeGeschwindigkeit(speed,0,0,0);
        }

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
            DuTLogger::logMessage("CAN Connector: BMW codec received unknown operation: <" + event.operation + ">", LOG_LEVEL::WARNING);
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

            case 0x275:
                // Geschwindigkeit CAN frame

                events = decodeGeschwindigkeit(frame, isCanfd);
                break;

            case 0x273:
                // GPS_LOCA CAN frame

                events = decodeGPS_LOCA(frame, isCanfd);
                break;

            case 0x274:
                // GPS_LOCB CAN frame

                events = decodeGPS_LOCB(frame, isCanfd);
                break;

            case 0x279:
                // Lichter CAN frame

                events = decodeLichter(frame, isCanfd);
                break;

            default:

                DuTLogger::logMessage("CAN Connector: BMW codec did not implement a conversion for the CAN ID: "
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

    std::vector<SimEvent> BmwCodec::decodeGeschwindigkeit(struct canfd_frame frame, bool isCanfd){

        struct can_frame canframe = {0};
        std::vector<SimEvent> events;

        // Check if we got a CAN frame
        if(isCanfd){
            DuTLogger::logMessage("Got a CANFD frame but expected a CAN frame for the CAN ID 0x275 Geschwindigkeit", LOG_LEVEL::ERROR);
            return events;
        }

        struct can_frame canFrame =  *((struct can_frame*) &frame);

        // Get the raw values that should be in little endian order
        uint16_t rawSpeed           = canFrame.data[0] | (canFrame.data[1] << 8);
        uint16_t rawAngularVelocity = canFrame.data[2] | (canFrame.data[3] << 8);
        uint16_t rawAccelerationY   = canFrame.data[4] | (canFrame.data[5] << 8);
        uint16_t rawAccelerationX   = canFrame.data[6] | (canFrame.data[7] << 8);

        // Convert to host order
        if(hostIsBigEndian){
            rawSpeed           = CodecUtilities::convertEndianness(rawSpeed);
            rawAngularVelocity = CodecUtilities::convertEndianness(rawAngularVelocity);
            rawAccelerationY   = CodecUtilities::convertEndianness(rawAccelerationY);
            rawAccelerationX   = CodecUtilities::convertEndianness(rawAccelerationX);
        }

        // Apply the scaling and offset
        uint16_t realSpeed           = rawSpeed           * V_VEHCOG_SCALING + V_VEHCOG_OFFSET;
        uint16_t realAngularVelocity = rawAngularVelocity * VYAWVEH_SCALING  + VYAWVEH_OFFSET;
        uint16_t realAccelerationY   = rawAccelerationY   * ACLNYCOG_SCALING + ACLNYCOG_OFFSET;
        uint16_t realAccelerationX   = rawAccelerationX   * ACLNXCOG_SCALING + ACLNXCOG_OFFSET;

        // Create the SimEvents and add them to the vector that will be sent to the simulation
        // TODO: The Simulation has only one acceleration and not an acceleration for X and Y!
        SimEvent speed  = SimEvent("Speed_DUT", static_cast<double>(realSpeed),"CanConnector");
        SimEvent yawRateDynamics = SimEvent("YawRate_Dynamics", static_cast<double>(rawAngularVelocity),"CanConnector");

        events.push_back(speed);
        events.push_back(yawRateDynamics);

        return events;
    }

    std::vector<SimEvent> BmwCodec::decodeGPS_LOCA(struct canfd_frame frame, bool isCanfd){

        std::vector<SimEvent> events;

        if(isCanfd){
            DuTLogger::logMessage("Got a CANFD frame but expected a CAN frame for the CAN ID 0x273 GPS_LOCA", LOG_LEVEL::ERROR);
            return events;
        }

        struct can_frame canFrame =  *((struct can_frame*) &frame);

        // Get the raw value that should be in little endian order
        uint32_t rawLongitude = canFrame.data[0] | (canFrame.data[1] << 8) | (canFrame.data[2] << 16) | (canFrame.data[3] << 24);
        uint32_t rawLatitude  = canFrame.data[4] | (canFrame.data[5] << 8) | (canFrame.data[6] << 16) | (canFrame.data[7] << 24);

        // Convert to host order
        if(hostIsBigEndian){
            rawLongitude = CodecUtilities::convertEndianness(rawLongitude);
            rawLatitude  = CodecUtilities::convertEndianness(rawLatitude);
        }

        // Apply the scaling and offset
        uint32_t realLongitude = rawLongitude * ST_LONGNAVI_SCALING + ST_LONGNAVI_OFFSET;
        uint32_t realLatitude  = rawLatitude  * ST_LATNAVI_SCALING  + ST_LATNAVI_OFFSET;

        // Create the SimEvents and add them to the vector that will be sent to the simulation
        SimEvent latitudeDynamics  = SimEvent("Latitude_Dynamics", static_cast<double>(realLatitude),"CanConnector");
        SimEvent longitudeDynamics = SimEvent("Longitude_Dynamics", static_cast<double>(realLongitude),"CanConnector");

        events.push_back(latitudeDynamics);
        events.push_back(longitudeDynamics);

        return events;
    }

    std::vector<SimEvent> BmwCodec::decodeGPS_LOCB(struct canfd_frame frame, bool isCanfd){

        std::vector<SimEvent> events;

        if(isCanfd){
            DuTLogger::logMessage("Got a CANFD frame but expected a CAN frame for the CAN ID 0x274 GPS_LOCB", LOG_LEVEL::ERROR);
            return events;
        }

        struct can_frame canFrame =  *((struct can_frame*) &frame);

        // Get the raw value that should be in little endian order
        uint16_t rawAltitude = canFrame.data[0] | (canFrame.data[1] << 8);
        uint8_t  rawHeading  = canFrame.data[2];
        uint8_t  rawDvcoveh  = canFrame.data[3];

        // Convert to host order
        if(hostIsBigEndian){
            rawAltitude = CodecUtilities::convertEndianness(rawAltitude);
            // No need to convert a uint8_t because it is a single byte
        }

        // Apply the scaling and offset
        uint16_t realAltitude = rawAltitude * ST_HGNAVI_SCALING        + ST_HGNAVI_OFFSET;
        uint8_t  realHeaind   = rawHeading  * ST_HDG_HRZTLABSL_SCALING + ST_HDG_HRZTLABSL_OFFSET;
        uint8_t  realDvcoveh  = rawDvcoveh  * DVCOVEH_SCALING          + DVCOVEH_OFFSET;

        // Create the SimEvents and add them to the vector that will be sent to the simulation
        // TODO:: z and y are written small here. This should be fixed on the simulation site maybe.
        SimEvent altitude  = SimEvent("Position_z-Coordinate_DUT", static_cast<double>(realAltitude),"CanConnector");
        SimEvent heading = SimEvent("Heading_Dynamics", static_cast<double>(realHeaind),"CanConnector");

        events.push_back(altitude);
        events.push_back(heading);

        return events;
    }

    std::vector<SimEvent> BmwCodec::decodeLichter(struct canfd_frame frame, bool isCanfd){

        std::vector<SimEvent> events;

        if(isCanfd){
            DuTLogger::logMessage("Got a CANFD frame but expected a CAN frame for the CAN ID 0x279 L", LOG_LEVEL::ERROR);
            return events;
        }

        struct can_frame canFrame =  *((struct can_frame*) &frame);

        // Get the raw value that should be in little endian order
        uint16_t rawsignals = canFrame.data[0] | (canFrame.data[1] << 8);

        // Create the SimEvents

        // Bit positions are counted from byte 0 upwards by their significance, regardless of the endianness.
        // The first message byte has bits 0…7 with bit 7 being the most significant bit of the byte

        // ST_PLILH (Blinker Right):
        // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
        // Mask:    0  0  0  0  1  0  0  0 | 0   0   0   0   0   0   0  0
        // Dec:     2048

        // ST_PLIRH (Blinker Left):
        // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
        // Mask:    0  0  0  1  0  0  0  0 | 0   0   0   0   0   0   0  0
        // Dec:     4096

        // ST_LOWBDAY (Daytime running lights):
        // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
        // Mask:    0  0  0  0  0  0  0  0 | 0   0   0   0   0   1   0  0
        // Dec:     4

        uint16_t simSignals = 0;

        if(rawsignals & 2048){

            // VEH_SIGNAL_BLINKER_RIGHT:
            // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
            // Mask:    0  0  0  0  0  0  0  1 | 0   0   0   0   0   0   0  0
            // Dec:     256
            simSignals = simSignals | 256;

        }
        if(rawsignals & 4096) {

            // VEH_SIGNAL_BLINKER_LEFT
            // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
            // Mask:    0  0  0  0  0  0  1  0 | 0   0   0   0   0   0   0  0
            // Dec:     256
            simSignals = simSignals | 512;
        }
        if(rawsignals & 4) {
            // TODO: No matching signal in the simulation only VEH_SIGNAL_FRONTLIGHT
            // => Abblendlicht != Tagfahrlicht
        }

        // Create the SimEvents and add them to the vector that will be sent to the simulation
        SimEvent signals  = SimEvent("Signals_DUT", static_cast<double>(simSignals),"CanConnector");
        events.push_back(signals);

        return events;
    }
}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/