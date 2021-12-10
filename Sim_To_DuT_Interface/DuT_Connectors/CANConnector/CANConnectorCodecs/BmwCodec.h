/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.cpp
 \brief     Implements the CAN codec for the BMW.
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_BMWCODEC_H
#define SIM_TO_DUT_INTERFACE_BMWCODEC_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "CodecUtilities.h"
#include "../CANConnectorCodec.h"

// System includes

/*******************************************************************************
 * DEFINES
 ******************************************************************************/

/**
 * Scaling and offset defines for the 0x275 Geschwindigkeit frame.
 */
#define V_VEHCOG_SCALING        0.015625
#define V_VEHCOG_OFFSET         0
#define VYAWVEH_SCALING         0.005
#define VYAWVEH_OFFSET         -163.84
#define ACLNYCOG_SCALING        0.002
#define ACLNYCOG_OFFSET        -65
#define ACLNXCOG_SCALING        0.002
#define ACLNXCOG_OFFSET        -65

/**
 * Scaling and offset defines for the 0x273 GPS_LOCA frame.
 */
#define ST_LONGNAVI_SCALING     0.00000008
#define ST_LONGNAVI_OFFSET      0
#define ST_LATNAVI_SCALING      0.00000008
#define ST_LATNAVI_OFFSET       0

/**
 * Scaling and offset defines for the 0x274 GPS_LOCB frame.
 */
#define ST_HGNAVI_SCALING        1
#define ST_HGNAVI_OFFSET         0
#define ST_HDG_HRZTLABSL_SCALING 1.5
#define ST_HDG_HRZTLABSL_OFFSET  0
#define DVCOVEH_SCALING          1
#define DVCOVEH_OFFSET           0


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class BmwCodec : public CANConnectorCodec{

    public:
        // Function members

        /**
         * Constructor.
         */
        BmwCodec();

        /**
        * Converts an simulation event to a CAN/CANFD payload.
        *
        * @param event - The simulation event we want to transform into a CAN/CANFD frame payload.
        *
        * @return The CAN/CANFD frame payload.
        */
        std::vector<__u8> convertSimEventToFrame(SimEvent event) override;

        /**
        * Converts a CAN/CANFD frame to the corresponding simulation events.
        *
        * @param frame   - The frame that we want to transform.
        * @param isCanfd - Flag for CANFD frames.
        *
        * @return The simulation events that were contained in the CAN/CANFD frame.
        */
        std::vector<SimEvent> convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd) override;

        // Data members

    private:
        // Function members

        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x275 Geschwindigkeit.
         *
         * @param speed           V_VEHCOG - SimEvent speed
         * @param angularVelocity VYAWVEH  - SimEvent ---
         * @param accelerationX   ACLNYCOG - SimEvent
         * @param accelerationY   ACLNXCOG - SimEvent
         * @return
         */
        std::vector<__u8> encodeGeschwindigkeit(uint16_t speed, uint16_t angularVelocity, uint16_t accelerationX, uint16_t accelerationY);


        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x273 GPS_LOCA.
         *
         * @param longitude ST_LONGNAVI
         * @param latidue   ST_LATNAVI
         * @return
         */
        std::vector<__u8> encodeGPS_LOCA(uint32_t longitude, uint32_t latitude);


        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x274 GPS_LOCB.
         *
         * @param altidute - SimEvent - Position_Z-Coordinate
         * @param heading  - SimEvent - HeadingDynamics
         * @param dvcoveh
         * @return
         */
        std::vector<__u8> encodeGPS_LOCB(uint16_t altitude, uint8_t heading, uint8_t dvcoveh);

        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x279 Lichter.
         *
         * @param singals
         * @return
         */
        std::vector<__u8> encodeLichter(uint16_t singals);


        /**
         * Decodes the CAN frame 0x275 Geschwindigkeit to the corresponding simulation events.
         *
         * @param frame   - The received CAN/CANFD frame
         * @param isCanfd - Flag for CANFD frames
         */
        std::vector<SimEvent> decodeGeschwindigkeit(struct canfd_frame frame, bool isCanfd);


        /**
         * Decodes the CAN frame 0x273 GPS_LOCA to the corresponding simulation events.
         *
         * @param frame   - The received CAN/CANFD frame
         * @param isCanfd - Flag for CANFD frames
         */
        std::vector<SimEvent> decodeGPS_LOCA(struct canfd_frame frame, bool isCanfd);


        /**
         * Decodes the CAN frame 0x274 GPS_LOCB to the corresponding simulation events.
         *
         * @param frame   - The received CAN/CANFD frame
         * @param isCanfd - Flag for CANFD frames
         */
        std::vector<SimEvent> decodeGPS_LOCB(struct canfd_frame frame, bool isCanfd);


        /**
         * Decodes the CAN frame 0x279 Lichter to the corresponding simulation events.
         *
         * @param frame   - The received CAN/CANFD frame
         * @param isCanfd - Flag for CANFD frames
         */
        std::vector<SimEvent> decodeLichter(struct canfd_frame frame, bool isCanfd);

        /**
         * Test/Demo-function.
         *
         * @param frame
         * @param isCanfd
         * @return
         */
        std::vector<SimEvent> handleHazardFrame(struct canfd_frame frame, bool isCanfd);

        /**
         * Test/Demo-function.
         *
         * @param frame
         * @param isCanfd
         * @return
         */
        std::vector<SimEvent> handleBrakeFrame(struct canfd_frame frame, bool isCanfd);

        // Data members
        bool hostIsBigEndian; /**< Flag if the system uses big endianness */
        std::unordered_map<std::string, SimEvent> currentGeschwindigkeitEvents = std::unordered_map<std::string, SimEvent>();
    };

}


#endif //SIM_TO_DUT_INTERFACE_BMWCODEC_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/