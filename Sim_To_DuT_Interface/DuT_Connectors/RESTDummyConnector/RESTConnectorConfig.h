/**
 * HIL - REST Dummy Connector
 * This the connector to interface with the REST Dummy DuT
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - REST Dummy Connector".
 *
 * "HIL - REST Dummy Connector" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - REST Dummy Connector" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - REST Dummy Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_RESTCONNECTORCONFIG_H
#define SIM_TO_DUT_INTERFACE_RESTCONNECTORCONFIG_H

#include <string>
#include <utility>
#include "../ConnectorConfig.h"
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
namespace sim_interface::dut_connector::rest_dummy {
    class  RESTConnectorConfig : public ConnectorConfig {
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & archive, const unsigned int version){
            archive & BOOST_SERIALIZATION_NVP(baseUrlDuT);
            archive & BOOST_SERIALIZATION_NVP(baseCallbackUrl);
            archive & BOOST_SERIALIZATION_NVP(port);
            archive & BOOST_SERIALIZATION_NVP(operations);
            archive &  BOOST_SERIALIZATION_NVP(periodicOperations);
            archive & BOOST_SERIALIZATION_NVP(periodicTimerEnabled);






        }
        template<class Archive>
        inline void load_construct_data(Archive & archive, RESTConnectorConfig * configPtr, const unsigned int version)
        {   std::string _baseUrlDuT;
            std::string _baseCallbackUrl;
            int _port;
            std::set<std::string> _operations;
           std::map<std::string, int> _periodicOperations;
            bool _periodicTimerEnabled;

            archive >> BOOST_SERIALIZATION_NVP(_baseUrlDuT);
            archive >>BOOST_SERIALIZATION_NVP (_baseCallbackUrl);
            archive >> BOOST_SERIALIZATION_NVP(_port);
            archive >> BOOST_SERIALIZATION_NVP(_operations);
            archive >> BOOST_SERIALIZATION_NVP(_periodicOperations);
            archive >>BOOST_SERIALIZATION_NVP( _periodicTimerEnabled);



            ::new (configPtr) RESTConnectorConfig ( _baseUrlDuT, _baseCallbackUrl,  _port,
                    _operations, _periodicOperations,  _periodicTimerEnabled );

        }
    public:
         RESTConnectorConfig(std::string baseUrlDuT, std::string baseCallbackUrl, int port,
                                     std::set<std::string> operations, std::map<std::string, int> periodicOperations = {},
                                     bool periodicTimerEnabled = false)
                : ConnectorConfig(std::move(operations),"RESTConnectorConfig", std::move(periodicOperations), periodicTimerEnabled),
                  baseUrlDuT(std::move(baseUrlDuT)), baseCallbackUrl(std::move(baseCallbackUrl)), port(port) {
           assert(!this->baseUrlDuT.empty()); // url cannot be empty
           assert(!this->baseCallbackUrl.empty()); // url cannot be empty
           assert(this->port > 0); // ports are only valid > 0
        }

      RESTConnectorConfig(): ConnectorConfig(){};




        /**
         * Base URL of DuT to send to
         */
        std::string baseUrlDuT;
        /**
         * URL to call from DuT to send events to simulation
         */
        std::string baseCallbackUrl;
        /**
         * Port to open the REST API on
         */
        int port;
    };
}

#endif //SIM_TO_DUT_INTERFACE_RESTCONNECTORCONFIG_H
