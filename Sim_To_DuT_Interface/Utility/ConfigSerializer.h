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

    /**
    * <summary>
    * ConfigSerializer: This class is for de-/ serializing the different connectorConfig types
    * and overwrites the boost:serialization methods
    * </summary>
    */

    class ConfigSerializer {
    public:
        /**
        * T: Templates for any type, so that the following methods can be used by them
        * method:deserialize
        * @param ifs: address of an input stringstream
        * @param objName: address of name of a object
        * @param obj: pointer of any type of object
        *
        * if: when the input string stream is available
        * exception handling: catching archive exception and start InterfaceLogger:
        *  iss: create an input stringstream
        *  ia: create an xml input archive with the input stringstream and no header
        *  deserialize the object
        *  catch the exception if the deserialisation failed and start logging
        *  else: start logging:
        */

        template<typename T>
        static void deserialize(std::istringstream &ifs, const std::string &objName, T *obj) {
            if (ifs.good()) {
                try {
                    std::istringstream iss;
                    boost::archive::xml_iarchive ia(ifs, boost::archive::no_header);
                    ia & boost::serialization::make_nvp(objName.c_str(), *obj);

                }
                catch (boost::archive::archive_exception const &e) {
                    InterfaceLogger::logMessage(std::string("ConfigSerializer: Exception on deserialize: ", e.what()),
                                                LOG_LEVEL::ERROR);
                }

            } else {
                InterfaceLogger::logMessage("ConfigSerializer: input filestream not good", LOG_LEVEL::ERROR);
            }
        }

        /**
        * T: Templates for any type, so that the following methods can be used by them
        * method:serialize
        * @param objName: address of a string --> name of the object
        * @param file: address of a string --> file
        * @param obj: pointer of any type of object
        *
        * ofs: create an output file stream
        * if: when the output file stream is available
        *  oa: create an xml output archive with the output stringstream and no header
        *  serialize now the object in the outputarchive and close file
        *  else: start logging
        */

        template<typename T>
        static void serialize(const std::string &file, const std::string &objName, T &obj) {
            std::ofstream ofs(file);
            if (ofs.good()) {
                {
                    boost::archive::xml_oarchive oa(ofs, boost::archive::no_header);
                    oa << boost::serialization::make_nvp(objName.c_str(), obj);
                }
                ofs.close();
            } else {
                InterfaceLogger::logMessage("ConfigSerializer: output filestream not good", LOG_LEVEL::ERROR);
            }
        }
    };
}

/**
* namespace boost::serialization: overwrite the boost:serialization methods
*/

namespace boost::serialization {

    /**
    * Implementation the boost serialization for ConnectorConfig
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of a ConnectorConfig object
    * @param version: constant unsigned int --> unused
    *  --> method is empty
    */

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::ConnectorConfig &config, const unsigned int version) {}

    /**
    * method: save_construct_data --> serialize ConnectorConfig
    * @param ar: address of an archive to serialize
    * @param config: pointer of a ConnectorConfig object to serialized
    * @param version: constant unsigned int --> unused
    *
    * serialize now the attributes of the ConnectorConfig object:
    * @param operations, periodicOperations, periodicTimerEnabled:
    * find tag in the serialized xml and get the same attribute via pointer
    */

    template<class Archive>
    inline void save_construct_data(Archive &ar, const sim_interface::dut_connector::ConnectorConfig *config,
                                    const unsigned int version) {
        ar & boost::serialization::make_nvp("operations", config->operations);
        ar & boost::serialization::make_nvp("periodicOperations", config->periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", config->periodicTimerEnabled);
    }

    /**
    * method: load_construct_data --> deserialize ConnectorConfig
    * @param ar: address of an archive to deserialize
    * @param instance: pointer of a ConnectorConfig object to deserialized
    * @param file_version: constant unsigned int --> unused
    *
    * @param _operations, _periodicOperations, _periodicTimerEnabled:
    * create helping attributes for deserializing
    * deserialize now the helping attributes of the ConnectorConfig object
    * overwrite the current object from class with the helping variables
    */

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::ConnectorConfig *instance,
                                    const unsigned int file_version) {
        std::set<std::string> _operations;
        std::map<std::string, int> _periodicOperations;
        bool _periodicTimerEnabled;

        ar >> boost::serialization::make_nvp("operations", _operations);
        ar >> boost::serialization::make_nvp("periodicOperations", _periodicOperations);
        ar >> boost::serialization::make_nvp("periodicTimerEnabled", _periodicTimerEnabled);

        ::new(instance)sim_interface::dut_connector::ConnectorConfig(_operations,
                                                                     _periodicOperations,
                                                                     _periodicTimerEnabled);
    }

    /**
    * Implementation the boost serialization for RESTConnectorConfig
    *
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of a RESTConnectorConfig object
    * @param version: constant unsigned int --> unused
    *  --> method is empty
    */


    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::rest_dummy::RESTConnectorConfig &config,
                   const unsigned int version) {}

    /**
    * method: save_construct_data --> serialize RESTConnectorConfig
    * @param ar: address of an archive to serialize
    * @param config: pointer of a RESTConnectorConfig object to serialize
    * @param version: constant unsigned int --> unused
    *
    * serialize now the attributes of the RESTConnectorConfig object:
    * @param baseUrlDuT, baseCallbackUrl, port, operations, periodicOperations, periodicTimerEnabled:
    * find tag in the serialized xml and get the same attribute via pointer
    */
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

    /**
    * method: load_construct_data --> deserialize RESTConnectorConfig
    * @param ar: address of an archive to deserialize
    * @param instance: pointer of a RESTConnectorConfig object to sdeerialize
    * @param file_version: constant unsigned int --> unused
    *
    * @param _baseUrlDuT, _baseCallbackUrl, _port, _operations, _periodicOperations, _periodicTimerEnabled:
    * create helping attributes for deserializing
    * deserialize now the helping attributes of the RESTConnectorConfig object
    * overwrite the current object from class with the helping variables
    */
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


    /**
    * Implementation the boost serialization for V2XConnectorConfig
    *
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of a V2XConnectorConfig object
    * @param version: constant unsigned int --> unused
    *  --> method is empty
    */

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::v2x::V2XConnectorConfig &config,
                   const unsigned int version) {}

    /**
    * method: save_construct_data --> serialize V2XConnectorConfig
    * @param ar: address of an archive to serialize
    * @param config: pointer of a V2XConnectorConfig object to serialize
    * @param version: constant unsigned int --> unused
    *
    * serialize now the attributes of the V2XConnectorConfig object:
    * @param ifname, ethernetFrameType:
    * find tag in the serialized xml and get the same attribute via pointer
    */

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::v2x::V2XConnectorConfig *config,
                        const unsigned int version) {

        ar & boost::serialization::make_nvp("ifname", config->ifname);
        ar & boost::serialization::make_nvp("ethernetFrameType", config->ethernetFrameType);

    }

    /**
     * method: load_construct_data --> deserialize V2XConnectorConfig
     * @param ar: address of an archive to deserialize
     * @param instance: pointer of a V2XConnectorConfig object to deserialize
     * @param file_version: constant unsigned int --> unused
     *
     * @param _ifname, _ethernetFrameType:
     * create helping attributes for serializing and creating a new xml-file for checking
     * deserialize now the helping attributes of the V2XConnectorConfig object
     * @param _helper: String for handling hex-values
     *
     * if the attribute ethernetFrameType is an hex-value
     * @param ss: Stringstream for handling hex-values
     * convert the string into hex value and put it into the stringstream
     * put stringstream into _ethernetFrameType
     * else
     * @param ss: Stringstream to put it into _ethernetFrameType
     * write the string into the stringstream
     * put stringstream into _ethernetFrameType
     *
     * overwrite the current object from class with the helping variables
     */

    template<class Archive>
    inline void
    load_construct_data(Archive &ar, sim_interface::dut_connector::v2x::V2XConnectorConfig *instance,
                        const unsigned int file_version) {
        std::string _ifname;

        unsigned short _ethernetFrameType;
        std::string helper;
        ar & boost::serialization::make_nvp("ifname", _ifname);
        ar & boost::serialization::make_nvp("ethernetFrameType", helper);


        //  Logic that the key can be Hex value
        if (boost::algorithm::contains(helper, "0x")) {
            std::stringstream ss;
            ss << std::hex << helper;
            ss >> _ethernetFrameType;
        } else {
            std::stringstream ss;
            ss << helper;
            ss >> _ethernetFrameType;
        }


        ::new(instance)sim_interface::dut_connector::v2x::V2XConnectorConfig(_ifname, _ethernetFrameType);
    }

}

#endif //SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H