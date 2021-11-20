/**
 * HIL - V2X Connector
 * This the connector to interface with the V2X DuT
 *
 * Copyright (C) 2021 Franziska Ihrler
 *
 * This file is part of "HIL - V2X Connector".
 *
 * "HIL - V2X Connector"" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - V2X Connector"" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - REST Dummy Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Franziska Ihrler
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_V2XCONFIG_H
#define SIM_TO_DUT_INTERFACE_V2XCONFIG_H

//TODO Add Cube Vanetza lib and delete this struct
struct ContextParams
{
    std::string multicastIp;
    int multicastPort;
    std::string ccuIp;
    int ccuPort;
    // vanetza::security::HashedId8 pseudonym;
    // vanetza::geonet::StationType stationType;
    bool gatekeeper = true; /*< DCC gatekeeper at link layer */
    bool softStateNeighbours = false; /*< enable soft-state neighbour state in location table (non standard) */
    bool fadingCbfCounters = false; /* use fading Contention-Based Forwarding counters (non standard) */
    bool publishLinkLayer = false; /* publish link layer messages (rx and tx) */
    double defaultTxPower = std::numeric_limits<double>::quiet_NaN();
};

namespace sim_interface::dut_connector::v2x{
    class V2XConfig : public ConnectorConfig {
    public:
        ContextParams params;
    };
}

#endif //SIM_TO_DUT_INTERFACE_V2XCONFIG_H