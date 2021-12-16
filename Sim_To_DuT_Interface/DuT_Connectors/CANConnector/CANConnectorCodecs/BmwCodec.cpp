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


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    BmwCodec::BmwCodec() : hostIsBigEndian(CodecUtilities::checkBigEndianness()){

        // Init cachedSimEventValues with zero

        // SimEvents for the 0x275 GESCHWINDIGKEIT frame
        cachedSimEventValues["Speed_Dynamics"] = 0;
        cachedSimEventValues["YawRate_Dynamics"] = 0;
        cachedSimEventValues["Acceleration_Dynamics"] = 0;

        // SimEvents for the 0x273 GPS_LOCA frame
        cachedSimEventValues["Latitude_Dynamics"] = 0;
        cachedSimEventValues["Longitude_Dynamics"] = 0;

        // SimEvents for the 0x274 GPS_LOCB frame
        cachedSimEventValues["Position_Z-Coordinate_DUT"] = 0;
        cachedSimEventValues["Heading_Dynamics"] = 0;

        // SimEvents for the  0x279 LICHTER frame
        // Does not need a cache because the frame can be build with only one SimEvent
    }

    std::pair<std::vector<__u8>, std::string> BmwCodec::convertSimEventToFrame(SimEvent event){

        auto result = std::pair<std::vector<__u8>, std::string>();

        if(event.operation == "Speed_Dynamics" || event.operation == "YawRate_Dynamics" || event.operation == "Acceleration_Dynamics"){
            result = encodeGeschwindigkeit(event);
        }else if(event.operation == "Latitude_Dynamics" || event.operation == "Longitude_Dynamics"){
            result = encodeGPS_LOCA(event);
        }else if(event.operation == "Position_Z-Coordinate_DUT" || event.operation == "Heading_Dynamics"){
            result = encodeGPS_LOCB(event);
        }else if(event.operation == "Signals_DUT"){
            result = encodeLichter(event);
        }else{
            InterfaceLogger::logMessage("CAN Connector: BMW codec received unknown operation: <" + event.operation + ">", LOG_LEVEL::WARNING);
        }

        return result;
    }

    std::vector<SimEvent> BmwCodec::convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd){

        // Vector that contains the resulting simulation events
        std::vector<SimEvent> events;

        switch(frame.can_id){

            case 0x111:

                // 0x275 GESCHWINDIGKEIT frame
                events = decodeGeschwindigkeit(frame, isCanfd);
                break;

            case 0x222:

                // 0x273 GPS_LOCA frame
                events = decodeGPS_LOCA(frame, isCanfd);
                break;

            case 0x333:

                // 0x274 GPS_LOCB frame
                events = decodeGPS_LOCB(frame, isCanfd);
                break;

            case 0x444:

                // 0x279 LICHTER frame
                events = decodeLichter(frame, isCanfd);
                break;

            default:

                InterfaceLogger::logMessage("CAN Connector: BMW codec did not implement a conversion for the CAN ID: "
                                      "<" + std::to_string(frame.can_id) + ">", LOG_LEVEL::WARNING);

        }

        return events;
    }

    std::pair<std::vector<__u8>, std::string> BmwCodec::encodeGeschwindigkeit(SimEvent event){

        auto result = std::pair<std::vector<__u8>, std::string>();

        if(event.value.type() != typeid(double)){
            throw std::invalid_argument("BMW Codec: SimEvent value type invalid");
        }

        // Cash the value of the event
        cachedSimEventValues[event.operation] = boost::get<double>(event.value);

        // Get the current event values
        double realSpeed           = cachedSimEventValues["Speed_Dynamics"];
        double realAngularVelocity = cachedSimEventValues["YawRate_Dynamics"];
        double realAccelerationY   = cachedSimEventValues["Acceleration_Dynamics"];
        double realAccelerationX   = 0;

        // Apply the scaling and offset
        auto rawSpeed           = (uint16_t) (realSpeed           / V_VEHCOG_SCALING - V_VEHCOG_OFFSET);
        auto rawAngularVelocity = (uint16_t) (realAngularVelocity / VYAWVEH_SCALING  -  VYAWVEH_OFFSET);
        auto rawAccelerationY   = (uint16_t) (realAccelerationY   / ACLNYCOG_SCALING - ACLNYCOG_OFFSET);
        auto rawAccelerationX   = (uint16_t) (realAccelerationX   / ACLNXCOG_SCALING - ACLNXCOG_OFFSET);

        // Convert to host order
        if(hostIsBigEndian){
            rawSpeed           = CodecUtilities::convertEndianness(rawSpeed);
            rawAngularVelocity = CodecUtilities::convertEndianness(rawAngularVelocity);
            rawAccelerationY   = CodecUtilities::convertEndianness(rawAccelerationY);
            rawAccelerationX   = CodecUtilities::convertEndianness(rawAccelerationX);
        }

        result.first = std::vector<__u8> {
            (uint8_t) rawSpeed, (uint8_t) (rawSpeed >> 8),
            (uint8_t)rawAngularVelocity, (uint8_t) (rawAngularVelocity >> 8),
            (uint8_t) rawAccelerationY, (uint8_t) (rawAccelerationY >> 8),
            (uint8_t) rawAccelerationX, (uint8_t) (rawAccelerationX >> 8),
            };

        result.second = GESCHWINDIGKEIT_SENDOPERATION;
        return result;
    }

    std::pair<std::vector<__u8>, std::string> BmwCodec::encodeGPS_LOCA(SimEvent event){

        auto result = std::pair<std::vector<__u8>, std::string>();

        if(event.value.type() != typeid(double)){
            throw std::invalid_argument("BMW Codec: SimEvent value type invalid");
        }

        // Cash the value of the event
        cachedSimEventValues[event.operation] = boost::get<double>(event.value);

        // Get the current event values
        double realLongitude = cachedSimEventValues["Longitude_Dynamics"];
        double realLatitude  = cachedSimEventValues["Latitude_Dynamics"];

        // Apply the scaling and offset
        auto rawLongitude = (int32_t) (realLongitude / ST_LONGNAVI_SCALING - ST_LONGNAVI_OFFSET);
        auto rawLatitude  = (int32_t) (realLatitude  / ST_LATNAVI_SCALING  - ST_LATNAVI_OFFSET);

        // Convert to host order
        if(hostIsBigEndian){
            rawLongitude = CodecUtilities::convertEndianness(rawLongitude);
            rawLatitude  = CodecUtilities::convertEndianness(rawLatitude);
        }

        result.first = std::vector<__u8>{
                (uint8_t) rawLongitude, (uint8_t) (rawLongitude >> 8),
                (uint8_t) (rawLongitude >> 16),(uint8_t) (rawLongitude >> 24),
                (uint8_t) rawLatitude, (uint8_t) (rawLatitude >> 8),
                (uint8_t) (rawLatitude >> 16),(uint8_t) (rawLatitude >> 24),
        };

        result.second = GPS_LOCA_SENDOPERATION;
        return result;
    }

    std::pair<std::vector<__u8>, std::string> BmwCodec::encodeGPS_LOCB(SimEvent event){
        auto result = std::pair<std::vector<__u8>, std::string>();

        if(event.value.type() != typeid(double)){
            throw std::invalid_argument("BMW Codec: SimEvent value type invalid");
        }
        cachedSimEventValues[event.operation] = boost::get<double>(event.value);

        double realAltitude = cachedSimEventValues["Position_Z-Coordinate_DUT"];
        double realHeading = cachedSimEventValues["Heading_Dynamics"];
        double realDvcoveh = 0;

        auto rawAltitude = (int16_t) (realAltitude / ST_HGNAVI_SCALING - ST_HGNAVI_OFFSET);
        auto rawHeading = (uint8_t) (realHeading / ST_HDG_HRZTLABSL_SCALING - ST_HDG_HRZTLABSL_OFFSET);
        auto rawDvcoveh = (uint8_t) (realDvcoveh / DVCOVEH_SCALING - DVCOVEH_OFFSET);

        // Convert to host order
        if(hostIsBigEndian){
            rawAltitude = CodecUtilities::convertEndianness(rawAltitude);
            // No need to convert an uint8_t because it is a single byte
        }

        result.first = std::vector<__u8>{
                (uint8_t) rawAltitude, (uint8_t) (rawAltitude >> 8),
                (uint8_t) rawHeading,
                (uint8_t) rawDvcoveh,
        };
        result.second = GPS_LOCB_SENDOPERATION;
        return result;
    }

    std::pair<std::vector<__u8>, std::string> BmwCodec::encodeLichter(SimEvent event){
        auto result = std::pair<std::vector<__u8>, std::string>();
        result.second = LICHTER_SENDOPERATION;

        if(event.value.type() != typeid(int)){
            throw std::invalid_argument("BMW Codec: SimEvent value type invalid");
        }

        uint16_t rawSignals = boost::get<int>(event.value);
        uint8_t canDataByte1 = 0;
        uint8_t canDataByte2 = 0;

        // Bit Pos traci: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8

        // Blinker Right    sumo bit 0      bmw bit 4
        if(rawSignals & 0x0001){
            canDataByte1 |= 0x10;
        }
        // Blinker Left     sumo bit 1      bmw bit 3
        if(rawSignals & 0x0002){
            canDataByte1 |= 0x04;
        }
        // Tagfahrlicht     sumo bit 4      bmw bit 10
        if(rawSignals & 0x0008){
            canDataByte2 |= 0x04;
        }

        result.first = std::vector<__u8> { canDataByte1, canDataByte2};
        return result;
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
        // The simulation has only one acceleration. We only use the Y acceleration.
        SimEvent speed  = SimEvent("Speed_DUT", static_cast<double>(realSpeed),"CanConnector");
        SimEvent yawRateDynamics = SimEvent("YawRate_Dynamics", static_cast<double>(realAngularVelocity),"CanConnector");
        SimEvent accelerationDynamics = SimEvent("Acceleration_Dynamics", static_cast<double>(realAccelerationY), "CanConnector");

        events.push_back(speed);
        events.push_back(yawRateDynamics);
        events.push_back(accelerationDynamics);

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
        int32_t rawLongitude = canFrame.data[0] | (canFrame.data[1] << 8) | (canFrame.data[2] << 16) | (canFrame.data[3] << 24);
        int32_t rawLatitude  = canFrame.data[4] | (canFrame.data[5] << 8) | (canFrame.data[6] << 16) | (canFrame.data[7] << 24);

        // Convert to host order
        if(hostIsBigEndian){
            rawLongitude = CodecUtilities::convertEndianness(rawLongitude);
            rawLatitude  = CodecUtilities::convertEndianness(rawLatitude);
        }

        // Apply the scaling and offset
        int32_t realLongitude = rawLongitude * ST_LONGNAVI_SCALING + ST_LONGNAVI_OFFSET;
        int32_t realLatitude  = rawLatitude  * ST_LATNAVI_SCALING  + ST_LATNAVI_OFFSET;

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
        int16_t rawAltitude = canFrame.data[0] | (canFrame.data[1] << 8);
        uint8_t  rawHeading  = canFrame.data[2];
        uint8_t  rawDvcoveh  = canFrame.data[3];

        // Convert to host order
        if(hostIsBigEndian){
            rawAltitude = CodecUtilities::convertEndianness(rawAltitude);
            // No need to convert a uint8_t because it is a single byte
        }

        // Apply the scaling and offset
        int16_t realAltitude  = rawAltitude * ST_HGNAVI_SCALING        + ST_HGNAVI_OFFSET;
        uint8_t  realHeaing   = rawHeading  * ST_HDG_HRZTLABSL_SCALING + ST_HDG_HRZTLABSL_OFFSET;
        uint8_t  realDvcoveh  = rawDvcoveh  * DVCOVEH_SCALING          + DVCOVEH_OFFSET;

        // Create the SimEvents and add them to the vector that will be sent to the simulation
        SimEvent altitude  = SimEvent("Position_Z-Coordinate_DUT", static_cast<double>(realAltitude),"CanConnector");
        SimEvent heading = SimEvent("Heading_Dynamics", static_cast<double>(realHeaing),"CanConnector");

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

        // DBC and traci:
        // Bit positions are counted from byte 0 upwards by their significance, regardless of the endianness.
        // The first message byte has bits 0…7 with bit 7 being the most significant bit of the byte

        // ST_PLILH (Blinker Right):
        // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
        // Mask:    0  0  0  0  1  0  0  0 | 0   0   0   0   0   0   0  0

        // ST_PLIRH (Blinker Left):
        // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
        // Mask:    0  0  0  1  0  0  0  0 | 0   0   0   0   0   0   0  0

        // ST_LOWBDAY (Daytime running lights):
        // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
        // Mask:    0  0  0  0  0  0  0  0 | 0   0   0   0   0   1   0  0

        uint16_t simSignals = 0;
        if(rawsignals & 0x0800){
            // VEH_SIGNAL_BLINKER_RIGHT:
            // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
            // Mask:    0  0  0  0  0  0  0  1 | 0   0   0   0   0   0   0  0
            simSignals |= 0x0100;
        }
        if(rawsignals & 0x1000) {
            // VEH_SIGNAL_BLINKER_LEFT
            // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
            // Mask:    0  0  0  0  0  0  1  0 | 0   0   0   0   0   0   0  0
            simSignals |= 0x0200;
        }
        if(rawsignals & 0x0004) {
            // VEH_SIGNAL_FRONTLIGHT
            // Bit Pos: 7  6  5  4  3  2  1  0 | 15  14  13  12  11  10  9  8
            // Mask:    0  0  0  1  0  0  0  0 | 0   0   0   0   0   0   0  0
            simSignals |= 0x1000;
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