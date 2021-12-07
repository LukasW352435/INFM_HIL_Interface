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
 * @version 1.0
 */


#ifndef SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H
#define SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <fstream>
#include <linux/can.h>
#include <linux/can/bcm.h>

#include "../DuT_Connectors/RESTDummyConnector/RESTConnectorConfig.h"
#include "../DuT_Connectors/CANConnector/CANConnectorConfig.h"
#include "../DuT_Connectors/CANConnector/CANConnectorReceiveOperation.h"
#include "../DuT_Connectors/CANConnector/CANConnectorSendOperation.h"
#include "../DuT_Connectors/ConnectorConfig.h"
#include "../DuTLogger/DuTLogger.h"

namespace sim_interface {

    class ConfigSerializer {
        public:

            template<typename T>
            static void deserialize( const std::string &file, const std::string &objName, boost::scoped_ptr<T> &obj) {
                std::ifstream ifs(file);
                //  std::ifstream ifs(file);
                //   boost::archive::xml_iarchive ia(ifs,boost::archive::no_header);
                //   ia & boost::serialization::make_nvp(objName.c_str(), obj);
                boost::archive::xml_iarchive ia(ifs,boost::archive::no_header);
                ia & boost::serialization::make_nvp(objName.c_str(), obj);


                if (ifs.good()) {
                    try {

                    }
                    catch (boost::archive::archive_exception const &e) {
                        DuTLogger::logMessage(std::string("ConfigSerializer: Exception on deserialize: ", e.what()),
                                              LOG_LEVEL::ERROR);
                    }

                    //   ifs.close();
                }

                else {
                    DuTLogger::logMessage("ConfigSerializer: input filestream not good", LOG_LEVEL::ERROR);
                }
            }
            /*
            static void deserialize( std::istringstream &ifs, const std::string &objName, boost::scoped_ptr<T> &obj) {
              //  std::ifstream ifs(file);
             //   boost::archive::xml_iarchive ia(ifs,boost::archive::no_header);
             //   ia & boost::serialization::make_nvp(objName.c_str(), obj);
                boost::archive::xml_iarchive ia(ifs,boost::archive::no_header);
                  ia & boost::serialization::make_nvp(objName.c_str(), obj);


                if (ifs.good()) {
                    try {

                    }
                    catch (boost::archive::archive_exception const &e) {
                        DuTLogger::logMessage(std::string("ConfigSerializer: Exception on deserialize: ", e.what()),
                                          LOG_LEVEL::ERROR);
                    }

                 //   ifs.close();
                }

                else {
                    DuTLogger::logMessage("ConfigSerializer: input filestream not good", LOG_LEVEL::ERROR);
                }
            }
            */



            template<typename T>
            static void serialize(const std::string &file, const std::string &objName, boost::scoped_ptr<T> &obj) {
                std::ofstream ofs(file);
                if (ofs.good()) {
                {
                    boost::archive::xml_oarchive oa(ofs, boost::archive::no_header);
                    oa << boost::serialization::make_nvp(objName.c_str(), obj);
                }
                ofs.close();
                }
                else {
                    DuTLogger::logMessage("ConfigSerializer: output filestream not good", LOG_LEVEL::ERROR);
                }
            }
    };
}

namespace boost::serialization {

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
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::rest_dummy::RESTConnectorConfig *instance,
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

        ::new(instance)sim_interface::dut_connector::ConnectorConfig(_operations,"ConnectorConfig" ,_periodicOperations,
                                                                     _periodicTimerEnabled);
        }

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorConfig &config,
                   const unsigned int version) {}

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::can::CANConnectorConfig *config,
                        const unsigned int version) {
        std::cout << "serialize" << std::endl;


        ar & boost::serialization::make_nvp("interfaceName", config->interfaceName);
        ar & boost::serialization::make_nvp("codecName", config->codecName);
        ar & boost::serialization::make_nvp("operations", config->operations);
        ar & boost::serialization::make_nvp("frameToOperation", config->frameToOperation);
        ar & boost::serialization::make_nvp("operationToFrame",  config->operationToFrame);
        ar & boost::serialization::make_nvp("periodicOperations", config->periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", config->periodicTimerEnabled);
    }

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorConfig *instance,
                                    const unsigned int file_version) {
        std::cout << "deserialize" << std::endl;
        std::string _interfaceName;
        std::string _codecName;
        std::set<std::string> _operations;
        std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> _frameToOperation = {};
        std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> _operationToFrame = {};
        std::map<std::string, int> _periodicOperations;
        bool _periodicTimerEnabled;

        ar & boost::serialization::make_nvp("interfaceName", _interfaceName);
        ar & boost::serialization::make_nvp("codecName", _codecName);
        ar & boost::serialization::make_nvp("operations", _operations);
        ar & boost::serialization::make_nvp("frameToOperation", _frameToOperation);
        ar & boost::serialization::make_nvp("operationToFrame", _operationToFrame);
        ar & boost::serialization::make_nvp("periodicOperations", _periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", _periodicTimerEnabled);


        ::new(instance)sim_interface::dut_connector::can::CANConnectorConfig(_interfaceName, _codecName,
                                                                                      _operations, _frameToOperation,
                                                                                     _operationToFrame,
                                                                                     _periodicOperations,
                                                                                     _periodicTimerEnabled);
    }


    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorReceiveOperation &config,
                   const unsigned int version) {}

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::can::CANConnectorReceiveOperation *config,
                        const unsigned int version) {



        ar & boost::serialization::make_nvp("operation", config->operation);
        ar & boost::serialization::make_nvp("isCANFD", config->isCANFD);
        ar & boost::serialization::make_nvp("hasMask", config->hasMask);
        ar & boost::serialization::make_nvp("maskLength", config->maskLength);
        ar & boost::serialization::make_nvp("mask.can_id)", config->mask.can_id);
        ar & boost::serialization::make_nvp("mask.len", config->mask.len);
        ar & boost::serialization::make_nvp("mask.flags", config->mask.flags);
        ar & boost::serialization::make_nvp("mask.__res0", config->mask.__res0);
        ar & boost::serialization::make_nvp("mask.__res1", config->mask.__res1);
        ar & boost::serialization::make_nvp("mask.data", config->mask.data);

    }

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorReceiveOperation *instance,
                                    const unsigned int file_version) {
        std::string _operation;
        bool _isCANFD;
        bool _hasMask;
        int _maskLength;
        __u8 *_maskData = nullptr;
        struct canfd_frame _mask = {0};


        ar & boost::serialization::make_nvp("operation", _operation);
        ar & boost::serialization::make_nvp("isCANFD", _isCANFD);
        ar & boost::serialization::make_nvp("hasMask", _hasMask);
        ar & boost::serialization::make_nvp("maskLength", _maskLength);
        ar & boost::serialization::make_nvp("mask.can_id)", _mask.can_id);
        ar & boost::serialization::make_nvp("mask.len", _mask.len);
        ar & boost::serialization::make_nvp("mask.flags", _mask.flags);
        ar & boost::serialization::make_nvp("mask.__res0", _mask.__res0);
        ar & boost::serialization::make_nvp("mask.__res1", _mask.__res1);
        ar & boost::serialization::make_nvp("mask.data", _mask.data);


        ::new(instance)sim_interface::dut_connector::can::CANConnectorReceiveOperation(_operation, _isCANFD,
                                                                             _hasMask, _maskLength,
                                                                             _maskData
                                                                             );
    }

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorSendOperation &config,
                   const unsigned int version) {}


    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::can::CANConnectorSendOperation *config,
                        const unsigned int version) {

        std::cout << "DESERIA" << std::endl;
      ar & boost::serialization::make_nvp("canID", config->canID);
      ar & boost::serialization::make_nvp("isCANFD", config->isCANFD);
      ar & boost::serialization::make_nvp("isCyclic", config->isCyclic);
      ar & boost::serialization::make_nvp("announce", config->announce);
      ar & boost::serialization::make_nvp("count)", config->count);
      ar & boost::serialization::make_nvp("ival1.tv_sec", config->ival1.tv_sec);
      ar & boost::serialization::make_nvp("ival1.tv_usec", config->ival1.tv_usec);
      ar & boost::serialization::make_nvp("mask.flags", config->ival2.tv_sec);
      ar & boost::serialization::make_nvp("mask.flags", config->ival2.tv_usec);

    }

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorSendOperation *instance,
                                    const unsigned int file_version) {
        canid_t _canID;
        bool _isCANFD;
        bool _isCyclic;
        bool _announce;
        __u32 _count;
        struct bcm_timeval _ival1;
        struct bcm_timeval _ival2;


        ar & boost::serialization::make_nvp("canID", _canID);
        ar & boost::serialization::make_nvp("isCANFD", _isCANFD);
        ar & boost::serialization::make_nvp("isCyclic", _isCyclic);
        ar & boost::serialization::make_nvp("announce", _announce);
        ar & boost::serialization::make_nvp("count)", _count);
        ar & boost::serialization::make_nvp("ival1.tv_sec", _ival1.tv_sec);
        ar & boost::serialization::make_nvp("ival1.tv_usec", _ival1.tv_usec);
        ar & boost::serialization::make_nvp("mask.flags", _ival2.tv_sec);
        ar & boost::serialization::make_nvp("mask.flags", _ival2.tv_usec);



        ::new(instance)sim_interface::dut_connector::can::CANConnectorSendOperation(_canID, _isCANFD,
                                                                                    _isCyclic, _announce,
                                                                                    _count, _ival1, _ival2
                                                                                    );
    }

    template <typename Ar>
    void serialize(Ar& ar, std::pair<int const, std::string>& p, unsigned) {
        std::cout << "WTF3" << std::endl;
        ar & make_nvp("key", p.first) & make_nvp("value", p.second);
    }
    template <class Archive>
    void serialize(Archive& ar, std::pair<const unsigned int, sim_interface::dut_connector::can::CANConnectorReceiveOperation>& p, unsigned) {
              std::cout << "WTF3" << std::endl;

    }
    template <class Archive>
    void serialize(Archive& ar, std::pair<const unsigned int, sim_interface::dut_connector::can::CANConnectorSendOperation>& p, unsigned) {
              std::cout << "WTF3" << std::endl;

    }
    template<class Archive>
    inline void load_construct_data(Archive &ar, std::pair<const unsigned int,  sim_interface::dut_connector::can::CANConnectorReceiveOperation> *pair,
                                    const unsigned int file_version) {
          int _key;
        std::cout << "WTF4" << std::endl;
        boost::scoped_ptr< sim_interface::dut_connector::can::CANConnectorReceiveOperation>  _value;
        ar &   boost::serialization::make_nvp("key", _key);
        ar &   boost::serialization::make_nvp("value",_value);
        std::pair<const unsigned int,  sim_interface::dut_connector::can::CANConnectorReceiveOperation> p2 = std::make_pair(_key,*_value);
        pair = &p2;
        //::new(pair)std::make_pair(_key,*_value);
    }
    template<class Archive>
    inline void load_construct_data(Archive &ar, std::pair<const std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *pair,
                                    const unsigned int file_version) {
        std::string _key;
        std::cout << "WTF3" << std::endl;
        boost::scoped_ptr< sim_interface::dut_connector::can::CANConnectorSendOperation>  _value;
        ar &   boost::serialization::make_nvp("key", _key);
        ar &   boost::serialization::make_nvp("value",_value);
      //  pair = std::make_pair(_key,*_value);
        std::pair<const std::string,  sim_interface::dut_connector::can::CANConnectorSendOperation> p2 = std::make_pair(_key,*_value);
        pair = &p2;
    }

    template<class Archive>
    inline void save_construct_data(Archive &ar, std::pair<const std::string , sim_interface::dut_connector::can::CANConnectorSendOperation> *pair,
                                    const unsigned int file_version) {
        std::cout << "WTF2" << std::endl;
        ar &   boost::serialization::make_nvp("key", pair->first);
       // ar &   boost::serialization::make_nvp("value", pair->second);

        ar &   boost::serialization::make_nvp("value", pair->second.ival1);
        ar &   boost::serialization::make_nvp("value", pair->second.ival2);
        ar &   boost::serialization::make_nvp("value", pair->second.isCANFD);
        ar &   boost::serialization::make_nvp("value", pair->second.count);
        ar &   boost::serialization::make_nvp("value", pair->second.announce);
        ar &   boost::serialization::make_nvp("value", pair->second.canID);
        ar &   boost::serialization::make_nvp("value", pair->second.isCyclic);

        std::cout << "DESERIA" << std::endl;

    }
    template<class Archive>
    inline void save_construct_data(Archive &ar, std::pair<const unsigned int, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *pair,
                                    const unsigned int file_version) {
        std::cout << "WTF1" << std::endl;
        ar &   boost::serialization::make_nvp("key", pair->first);
       // ar &   boost::serialization::make_nvp("value", pair->second);
        ar &   boost::serialization::make_nvp("value", pair->second.maskLength);
        ar &   boost::serialization::make_nvp("value", pair->second.mask);
        ar &   boost::serialization::make_nvp("value", pair->second.operation);
        ar &   boost::serialization::make_nvp("value", pair->second.hasMask);
        ar &   boost::serialization::make_nvp("value", pair->second.isCANFD);

    }


}

#endif //SIM_TO_DUT_INTERFACE_CONFIGSERIALIZER_H