/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Michael Schmitz
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
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @author Fabian Andre Genes
 * @author Thanaanncheyan Thavapalan
 * @version 1.0
 */


#ifndef SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H
#define SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>
//#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/algorithm//string.hpp>
#include <fstream>
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <boost/concept_check.hpp>

#include "../DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"
#include "../DuT_Connectors/V2XConnector/V2XConnector.h"
#include "../DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "../DuT_Connectors/CANConnector/CANConnectorReceiveOperation.h"
#include "../DuT_Connectors/CANConnector/CANConnectorSendOperation.h"
#include "../DuT_Connectors/ConnectorConfig.h"
#include "../Interface_Logger/InterfaceLogger.h"
#include "ConfigSerializerCanConnector.h"

namespace sim_interface {

    class ConfigSerializer {
    public:

        template<typename T>
        static void deserialize(const std::string &file, const std::string &objName, T *obj) {
            std::ifstream ifs(file);
            if (ifs.good()) {
                try {
                    //  std::ifstream ifs(file);
                    //   boost::archive::xml_iarchive ia(ifs,boost::archive::no_header);
                    //   ia & boost::serialization::make_nvp(objName.c_str(), obj);
                    boost::archive::xml_iarchive ia(ifs, boost::archive::no_header);
                    //  T* pointer;
                    ia & boost::serialization::make_nvp(objName.c_str(), obj);

                }
                catch (boost::archive::archive_exception const &e) {
                    InterfaceLogger::logMessage(std::string("ConfigSerializer: Exception on deserialize: ", e.what()),
                                          LOG_LEVEL::ERROR);
                }

                //   ifs.close();
            } else {
                InterfaceLogger::logMessage("ConfigSerializer: input filestream not good", LOG_LEVEL::ERROR);
            }
        }


        template<typename T>
        static void serialize(const std::string &file, const std::string &objName, T &obj) {
            std::ofstream ofs(file);
            if (ofs.good()) {
                {
                    boost::archive::xml_oarchive oa(ofs, boost::archive::no_header);
                    T *pointer = &obj;
                    oa << boost::serialization::make_nvp(objName.c_str(), pointer);
                }
                ofs.close();
            } else {
                InterfaceLogger::logMessage("ConfigSerializer: output filestream not good", LOG_LEVEL::ERROR);
            }
        }
    };
}

namespace boost::serialization {


    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::ConnectorConfig &config, const unsigned int version) {}

    template<class Archive>
    inline void save_construct_data(Archive &ar, const sim_interface::dut_connector::ConnectorConfig *config,
                                    const unsigned int version) {
        ar & boost::serialization::make_nvp("operations", config->operations);
        ar & boost::serialization::make_nvp("periodicOperations", config->periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", config->periodicTimerEnabled);
    }

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::ConnectorConfig *instance,
                                    const unsigned int file_version) {
        std::set<std::string> _operations;
        std::map<std::string, int> _periodicOperations;
        bool _periodicTimerEnabled;

        ar >> boost::serialization::make_nvp("operations", _operations);
        ar >> boost::serialization::make_nvp("periodicOperations", _periodicOperations);
        ar >> boost::serialization::make_nvp("periodicTimerEnabled", _periodicTimerEnabled);

        ::new(instance)sim_interface::dut_connector::ConnectorConfig(_operations, "ConnectorConfig",
                                                                     _periodicOperations,
                                                                     _periodicTimerEnabled);
    }

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::rest_dummy::RESTConnectorConfig &config,
                   const unsigned int version) {}

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *config,
                        const unsigned int version) {

        ar & boost::serialization::make_nvp("baseUrlDuT", config->baseUrlDuT);
        ar & boost::serialization::make_nvp("baseCallbackUrl", config->baseCallbackUrl);
        ar & boost::serialization::make_nvp("port", config->port);
        ar & boost::serialization::make_nvp("operations", config->operations);
        ar & boost::serialization::make_nvp("periodicOperations", config->periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", config->periodicTimerEnabled);
    }

    template<class Archive>
    inline void
    load_construct_data(Archive &ar, sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *instance,
                        const unsigned int file_version) {
        std::string _baseUrlDuT;
        std::string _baseCallbackUrl;
        int _port;
        std::set<std::string> _operations;
        std::map<std::string, int> _periodicOperations;
        bool _periodicTimerEnabled;

        ar >> boost::serialization::make_nvp("baseUrlDuT", _baseUrlDuT);
        ar >> boost::serialization::make_nvp("baseCallbackUrl", _baseCallbackUrl);
        ar >> boost::serialization::make_nvp("port", _port);
        ar >> boost::serialization::make_nvp("operations", _operations);
        ar >> boost::serialization::make_nvp("periodicOperations", _periodicOperations);
        ar >> boost::serialization::make_nvp("periodicTimerEnabled", _periodicTimerEnabled);

        ::new(instance)sim_interface::dut_connector::rest_dummy::RESTConnectorConfig(_baseUrlDuT, _baseCallbackUrl,
                                                                                     _port, _operations,
                                                                                     _periodicOperations,
                                                                                     _periodicTimerEnabled);
    }





    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::v2x::V2XConnectorConfig &config,
                   const unsigned int version) {}

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::v2x::V2XConnectorConfig *config,
                        const unsigned int version) {

        ar & boost::serialization::make_nvp("ifname", config->ifname);
        ar & boost::serialization::make_nvp("ethernetFrameType", config->ethernetFrameType);

    }

    template<class Archive>
    inline void
    load_construct_data(Archive &ar, sim_interface::dut_connector::v2x::V2XConnectorConfig *instance,
                        const unsigned int file_version) {
        std::string _ifname;

        unsigned short _ethernetFrameType;

        ar & boost::serialization::make_nvp("ifname", _ifname);
        ar & boost::serialization::make_nvp("ethernetFrameType", _ethernetFrameType);


        ::new(instance)sim_interface::dut_connector::v2x::V2XConnectorConfig(_ifname, _ethernetFrameType);
    }








}

#endif //SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H