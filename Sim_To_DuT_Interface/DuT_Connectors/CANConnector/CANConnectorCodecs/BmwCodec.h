/**
 * CAN Connector.
 * The Connector enables the communication over a CAN/CANFD interface.
 *
 * Copyright (C) 2021 Matthias Bank
 *
 * This file is part of "Sim To DuT Interface".
 *
 * "Sim To DuT Interface" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sim To DuT Interface" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sim To DuT Interface". If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Marco Keul
 * @author Lukas Wagenlehner
 * @author Matthias Bank
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_BMWCODEC_H
#define SIM_TO_DUT_INTERFACE_BMWCODEC_H

// Project includes
#include "CodecUtilities.h"
#include "../CANConnectorCodec.h"

// System includes
#include <map>

/**
 * Scaling and offset defines for the 0x275 GESCHWINDIGKEIT frame.
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

/**
 * SendOperation name for the 0x275 GESCHWINDIGKEIT frame.
 */
#define GESCHWINDIGKEIT_SENDOPERATION "GESCHWINDIGKEIT"

/**
 * SendOperation name for the 0x273 GPS_LOCA frame.
 */
#define GPS_LOCA_SENDOPERATION        "GPS_LOCA"

/**
 * SendOperation name for the 0x274 GPS_LOCB frame.
 */
#define GPS_LOCB_SENDOPERATION        "GPS_LOCB"

/**
 * SendOperation name for the 0x279 LICHTER frame.
 */
#define LICHTER_SENDOPERATION         "LICHTER"

namespace sim_interface::dut_connector::can {

    /**
     * <summary>
     * Implements the Codec for the BMW DuT.
     * </summary>
     */
    class BmwCodec : public CANConnectorCodec {

    public:

        /**
         * Constructor.
         */
        BmwCodec();

        /**
         * Converts an simulation event to a CAN/CANFD payload and determines the sendOperation name.
         *
         * @param event - The simulation event we want to transform into a CAN/CANFD frame payload.
         *
         * @return The CAN/CANFD frame payload and the sendOperation name
         */
        std::pair<std::vector<__u8>, std::string> convertSimEventToFrame(SimEvent event) override;

        /**
         * Converts a CAN/CANFD frame to the corresponding simulation events.
         *
         * @param frame   - The frame that we want to transform.
         * @param isCanfd - Flag for CANFD frames.
         *
         * @return The simulation events that were contained in the CAN/CANFD frame.
         */
        std::vector<SimEvent> convertFrameToSimEvent(struct canfd_frame frame, bool isCanfd) override;

    private:

        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x275 GESCHWINDIGKEIT and determines the sendOperation name.
         *
         * @param event - The simulation event.
         *
         * @return The payload for the CAN frame 0x275 GESCHWINDIGKEIT and the sendOperation name.
         */
        std::pair<std::vector<__u8>, std::string> encodeGeschwindigkeit(SimEvent event);

        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x273 GPS_LOCA and determines the sendOperation name.
         *
         * @param event - The simulation event.
         *
         * @retur The payload for the CAN frame 0x273 GPS_LOCA and the sendOperation name.
         */
        std::pair<std::vector<__u8>, std::string> encodeGPS_LOCA(SimEvent event);

        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x274 GPS_LOCB.
         *
         * @param event - The simulation event.
         *
         * @return The payload for the CAN frame 0x274 GPS_LOCB and determines the sendOperation name.
         */
        std::pair<std::vector<__u8>, std::string> encodeGPS_LOCB(SimEvent event);

        /**
         * Encodes the SimEvent values to a payload for the CAN frame 0x279 LICHTER and the sendOperation name.
         *
         * @param event - The simulation event.
         *
         * @return The payload for the CAN frame 0x279 LICHTER and determines the sendOperation name.
         */
        std::pair<std::vector<__u8>, std::string> encodeLichter(SimEvent event);

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
         * @param frame   - The received CAN/CANFD frame.
         * @param isCanfd - Flag for CANFD frames.
         */
        std::vector<SimEvent> decodeGPS_LOCA(struct canfd_frame frame, bool isCanfd);

        /**
         * Decodes the CAN frame 0x274 GPS_LOCB to the corresponding simulation events.
         *
         * @param frame   - The received CAN/CANFD frame.
         * @param isCanfd - Flag for CANFD frames.
         */
        std::vector<SimEvent> decodeGPS_LOCB(struct canfd_frame frame, bool isCanfd);

        /**
         * Decodes the CAN frame 0x279 Lichter to the corresponding simulation events.
         *
         * @param frame   - The received CAN/CANFD frame.
         * @param isCanfd - Flag for CANFD frames.
         */
        std::vector<SimEvent> decodeLichter(struct canfd_frame frame, bool isCanfd);

        bool hostIsBigEndian;                               /**< Flag if the system uses big endianness    */
        std::map<std::string, double> cachedSimEventValues; /**< Map to cache the previous SimEvent values */
    };

}

#endif //SIM_TO_DUT_INTERFACE_BMWCODEC_H