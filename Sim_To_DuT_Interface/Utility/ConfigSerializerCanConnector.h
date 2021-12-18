//
// Created by vagrant on 12/16/21.
//

#ifndef SIM_TO_DUT_INTERFACE_CONFIGSERIALIZERCANCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_CONFIGSERIALIZERCANCONNECTOR_H

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>

#include <boost/serialization/utility.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/algorithm//string.hpp>
#include <fstream>
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <boost/concept_check.hpp>

namespace boost::serialization {
    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorConfig &config,
                   const unsigned int version) {}

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::can::CANConnectorConfig *config,
                        const unsigned int version) {


        const std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *frameToOperationPointer = (&config->frameToOperation);
        const std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *operationToFramePointer = (&config->operationToFrame);

        ar & boost::serialization::make_nvp("interfaceName", config->interfaceName);
        ar & boost::serialization::make_nvp("codecName", config->codecName);
        ar & boost::serialization::make_nvp("operations", config->operations);
        ar & boost::serialization::make_nvp("frameToOperation", frameToOperationPointer);
        ar & boost::serialization::make_nvp("operationToFrame", operationToFramePointer);
        ar & boost::serialization::make_nvp("periodicOperations", config->periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", config->periodicTimerEnabled);
    }

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorConfig *instance,
                                    const unsigned int file_version) {

        std::string _interfaceName;
        std::string _codecName;
        std::set<std::string> _operations;


        std::map<std::string, int> _periodicOperations;
        bool _periodicTimerEnabled;


        std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *_frameToOperationPointer;

        std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *_operationToFramePointer = {};

        ar & boost::serialization::make_nvp("interfaceName", _interfaceName);
        ar & boost::serialization::make_nvp("codecName", _codecName);
        ar & boost::serialization::make_nvp("operations", _operations);
        ar & boost::serialization::make_nvp("frameToOperation", _frameToOperationPointer);
        ar & boost::serialization::make_nvp("operationToFrame", _operationToFramePointer);
        ar & boost::serialization::make_nvp("periodicOperations", _periodicOperations);
        ar & boost::serialization::make_nvp("periodicTimerEnabled", _periodicTimerEnabled);


        ::new(instance)sim_interface::dut_connector::can::CANConnectorConfig(_interfaceName, _codecName,
                                                                             _operations, *_frameToOperationPointer,
                                                                             *_operationToFramePointer,
                                                                             _periodicOperations,
                                                                             _periodicTimerEnabled);
    }


    template<class Archive>
    void
    serialize(Archive &ar, std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> &config,
              unsigned) {
    }

    template<class Archive>
    void save_construct_data(Archive &ar,
                             const std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *config,
                             unsigned) {
        unsigned long count = config->size();
        ar & boost::serialization::make_nvp("count", count);
        for (auto const &iter: *config) {

            //Don't work if we define the type of iter in the for loop. Then only the first element gets saved
            auto *pointer = reinterpret_cast<const std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *>(&iter);

            ar & boost::serialization::make_nvp("itemOperationToFrame", pointer);

        }

    }

    template<class Archive>
    void load_construct_data(Archive &ar,
                             std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *config,
                             unsigned) {


        std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *_pair;
        unsigned long _count;
        std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> _map = {};
        ar & boost::serialization::make_nvp("count", _count);
        for (int i = 0; i < _count; i++) {
            ar & boost::serialization::make_nvp("itemOperationToFrame", _pair);
            _map.insert(*_pair);
        }

        ::new(config)std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation>(_map);


    }


    template<class Archive>
    void serialize(Archive &ar,
                   std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> &p,
                   unsigned) {
    }


    template<class Archive>
    inline void save_construct_data(Archive &ar,
                                    const std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *pair,
                                    const unsigned int file_version) {


        const sim_interface::dut_connector::can::CANConnectorSendOperation *pointer = &pair->second;
        ar & boost::serialization::make_nvp("operation", pair->first);
        ar & boost::serialization::make_nvp("CANConnectorSendOperation", pointer);


    }


    template<class Archive>
    inline void load_construct_data(Archive &ar,
                                    std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *pair,
                                    const unsigned int file_version) {


        std::string _key;
        sim_interface::dut_connector::can::CANConnectorSendOperation *_value;


        ar & boost::serialization::make_nvp("operation", _key);
        ar & boost::serialization::make_nvp("CANConnectorSendOperation", _value);


        ::new(pair)std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation>(_key, *_value);


    }


    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorSendOperation &config,
                   unsigned) {
    }


    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::can::CANConnectorSendOperation *instance,
                        const unsigned int file_version) {


        ar & boost::serialization::make_nvp("canID", instance->canID);
        ar & boost::serialization::make_nvp("isCANFD", instance->isCANFD);
        ar & boost::serialization::make_nvp("isCyclic", instance->isCyclic);
        ar & boost::serialization::make_nvp("announce", instance->announce);
        ar & boost::serialization::make_nvp("countIval1", instance->count);
        ar & boost::serialization::make_nvp("ival1", instance->ival1);
        ar & boost::serialization::make_nvp("ival2", instance->ival2);

    }

    template<class Archive>
    void serialize(Archive &ar, bcm_timeval &config,
                   const unsigned int version) {
        ar & boost::serialization::make_nvp("tv_sec", config.tv_sec);
        ar & boost::serialization::make_nvp("tv_usec", config.tv_usec);

    }

    template<class Archive>
    inline void load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorSendOperation *instance,
                                    const unsigned int file_version) {


        std::string helper;
        canid_t _canID;
        bool _isCANFD;
        bool _isCyclic;
        bool _announce;
        __u32 _count;
        struct bcm_timeval _ival1 = {0};
        struct bcm_timeval _ival2 = {0};


        ar & boost::serialization::make_nvp("canID", helper);
        ar & boost::serialization::make_nvp("isCANFD", _isCANFD);
        ar & boost::serialization::make_nvp("isCyclic", _isCyclic);
        ar & boost::serialization::make_nvp("announce", _announce);
        ar & boost::serialization::make_nvp("countIval1", _count);
        ar & boost::serialization::make_nvp("ival1", _ival1);
        ar & boost::serialization::make_nvp("ival2", _ival2);


        //  Logic that the key can be Hex value
        if (boost::algorithm::contains(helper, "0x")) {
            std::stringstream ss;
            ss << std::hex << helper;
            ss >> _canID;
        } else {
            std::stringstream ss;
            ss << helper;
            ss >> _canID;
        }


        ::new(instance)sim_interface::dut_connector::can::CANConnectorSendOperation(_canID, _isCANFD,
                                                                                    _isCyclic, _announce,
                                                                                    _count, _ival1, _ival2
        );
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
        ar & boost::serialization::make_nvp("mask", config->mask.data);


    }


    template<class Archive>
    inline void
    load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorReceiveOperation *instance,
                        const unsigned int file_version) {

        std::string _operation;
        bool _isCANFD;
        bool _hasMask;
        int _maskLength;

        __u8 _mask[CANFD_MAX_DLEN];


        ar & boost::serialization::make_nvp("operation", _operation);
        ar & boost::serialization::make_nvp("isCANFD", _isCANFD);
        ar & boost::serialization::make_nvp("hasMask", _hasMask);
        ar & boost::serialization::make_nvp("maskLength", _maskLength);
        ar & boost::serialization::make_nvp("mask", _mask);


        ::new(instance)sim_interface::dut_connector::can::CANConnectorReceiveOperation(_operation, _isCANFD,
                                                                                       _hasMask, _maskLength,
                                                                                       _mask
        );
    }

    template<class Archive>
    void
    serialize(Archive &ar, std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> &config,
              unsigned) {
    }

    template<class Archive>
    void save_construct_data(Archive &ar,
                             const std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *config,
                             unsigned) {

        unsigned long count = config->size();
        ar & boost::serialization::make_nvp("count", count);
        for (auto const &iter: *config) {

            //Don't work if we define the type of iter in the for loop. Then only the first element gets saved
            const auto *pointer = reinterpret_cast<const std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *>(&iter);

            ar & boost::serialization::make_nvp("itemFrameToOperation", pointer);

        }

    }

    template<class Archive>
    void load_construct_data(Archive &ar,
                             std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *config,
                             unsigned) {

        std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *_pair;
        unsigned long _count;
        std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> _map = {};
        ar & boost::serialization::make_nvp("count", _count);
        for (int i = 0; i < _count; i++) {
            ar & boost::serialization::make_nvp("itemFrameToOperation", _pair);
            _map.insert(*_pair);
        }

        ::new(config)std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation>(_map);


    }


    template<class Archive>
    void serialize(Archive &ar, std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> &p,
                   unsigned) {

    }


    template<class Archive>
    inline void save_construct_data(Archive &ar,
                                    const std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *pair,
                                    const unsigned int file_version) {


        const sim_interface::dut_connector::can::CANConnectorReceiveOperation *pointer = &pair->second;
        ar & boost::serialization::make_nvp("canid", pair->first);
        ar & boost::serialization::make_nvp("CANConnectorReceiveOperation", pointer);

    }

    template<class Archive>
    inline void load_construct_data(Archive &ar,
                                    std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *pair,
                                    const unsigned int file_version) {
        unsigned int _key;
        std::string helper;


        sim_interface::dut_connector::can::CANConnectorReceiveOperation *_value;


        ar & boost::serialization::make_nvp("canid", helper);
        ar & boost::serialization::make_nvp("CANConnectorReceiveOperation", _value);

        //  Logic that the key can be Hex value
        if (boost::algorithm::contains(helper, "0x")) {
            std::stringstream ss;
            ss << std::hex << helper;
            ss >> _key;
        } else {
            std::stringstream ss;
            ss << helper;
            ss >> _key;
        }


        ::new(pair)std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation>(_key, *_value);


    }


}

#endif //SIM_TO_DUT_INTERFACE_CONFIGSERIALIZERCANCONNECTOR_H
