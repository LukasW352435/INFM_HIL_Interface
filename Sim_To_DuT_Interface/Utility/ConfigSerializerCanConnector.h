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

/**
* ConfigSerializerCanConnector: These methods are for de-/ serializing the CANConnectorConfig
* namespace boost::serialization: overwrite the boost:serialization methods
*/

namespace boost::serialization {

    /**
    * Implementation the boost serialization for CANConnectorConfig
    * @param Archive: Templates for any class, so that the following methods can be used by them
    *
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of a CANConnectorConfig object
    * @param version: constant unsigned int --> unused
    *  --> method is empty
    */
    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorConfig &config,
                   const unsigned int version) {}

    /**
    * method: save_construct_data --> serialize CANConnectorConfig
    * @param ar: address of an archive to serialize
    * @param config: pointer of a CANConnectorConfig object to serialize
    * @param version: constant unsigned int --> unused
    * serialize now the attributes of the CANConnectorConfig object:
    * @param interfaceName, codecName, operations, *frameToOperationPointer, *operationToFramePointer, periodicOperations, periodicTimerEnabled:
    * find tag in the serialized xml and get the same attribute via pointer
    */

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

    /**
    * method: load_construct_data --> deserialize CANConnectorConfig
    * @param ar: address of an archive to deserialize
    * @param instance: pointer of a CANConnectorConfig object to deserialize
    * @param file_version: constant unsigned int --> unused
    *
    * @param _interfaceName, _codecName, _operations, *_frameToOperationPointer, *_operationToFramePointer, _periodicOperations, _periodicTimerEnabled:
    * create helping attributes for deserializing
    * deserialize now the helping attributes of the CANConnectorConfig object
    * overwrite the current object from class with the helping variables
    */
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

    /**
    * Implementation the boost serialization for the map with string and CANConnectorSendOperation
    *
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: serialize
    * @param ar: address of an archive
    * @param p: address of the pair with string and CANConnectorSendOperation
    * @param unsigned --> unused
    *  --> method is empty
    */

    template<class Archive>
    void
    serialize(Archive &ar, std::map<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> &config,
              unsigned) {
    }

    /**
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: save_construct_data --> serialize the map with string and CANConnectorSendOperation
    * @param ar: address of an archive to serialize
    * @param config: address of the map with string and CANConnectorSendOperation to serialize
    * @param unsigned --> unused
    *
    * @param count: contains the length of the config pointer(size of the map)
    * serialize count
    *
    *  for each loop: run through each address in the config pointer (map)
    *  @param pointer: const auto *pointer = make a pointer to a pair with the map and cast it
    *  serialize every element in the map
    */
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

    /**
   * method: load_construct_data --> deserialize the map with string and CANConnectorSendOperation
   * @param ar: address of an archive to deserialize
   * @param config: address of the map with string and CANConnectorSendOperation to deserialize
   * @param unsigned: unsigned int --> unused
   *
   * @param _pair, _count, _map:
   * create helping attributes for deserializing
   * deserialize count
   * for loop:
   *   deserialize every element in the pair
   *   insert the pair into helping map
   *
   * overwrite the current map with the helping variables
   */
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

    /**
    * Implementation the boost serialization for the map with string and CANConnectorSendOperation
    *
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: serialize
    * @param ar: address of an archive
    * @param p: address of the pair with string and CANConnectorSendOperation
    * @param unsigned --> unused
    *  --> method is empty
    */

    template<class Archive>
    void serialize(Archive &ar,
                   std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> &p,
                   unsigned) {
    }

    /**
    * method: save_construct_data --> serialize the pair with string and CANConnectorSendOperation
    * @param ar: address of an archive to serialize
    * @param pair: address of the pair with string and CANConnectorSendOperation to serialize
    * @param file_version --> unused
    * @param pointer: *pointer form type CANConnectorSendOperation and save the second values of CANConnectorSendOperation via pair
    * serialize operation and CANConnectorSendOperation
    */

    template<class Archive>
    inline void save_construct_data(Archive &ar,
                                    const std::pair<std::string, sim_interface::dut_connector::can::CANConnectorSendOperation> *pair,
                                    const unsigned int file_version) {

        const sim_interface::dut_connector::can::CANConnectorSendOperation *pointer = &pair->second;
        ar & boost::serialization::make_nvp("operation", pair->first);
        ar & boost::serialization::make_nvp("CANConnectorSendOperation", pointer);
    }

    /**
    * method: load_construct_data --> deserialize the map with string and CANConnectorSendOperation
    * @param ar: address of an archive to deserialize
    * @param pair: address of the pair with string and CANConnectorSendOperation to deserialize
    * @param file_version: unsigned int --> unused
    *
    * @param _key, *_value:
    * create helping attributes for serializing and creating a new xml-file for checking
    * deserialize _key:operation
    * deserialize _value:CANConnectorSendOperation
    * overwrite the current pair with the helping variables
    */

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

    /**
    * Implementation the boost serialization for CANConnectorSendOperation
    * @param Archive: Templates for any class, so that the following methods can be used by them
    *
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of a CANConnectorSendOperation object
    * @param unsigned: unsigned --> unused
    *  --> method is empty
    */

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorSendOperation &config,
                   unsigned) {
    }

    /**
    * method: save_construct_data --> serialize CANConnectorSendOperation
    * @param ar: address of an archive to serialize
    * @param instance: pointer of a CANConnectorSendOperation object to serialize
    * @param version: constant unsigned int --> unused
    *
    * serialize now the attributes of the CANConnectorSendOperation object:
    * @param canID, isCANFD, isCyclic, announce, countIval1, ival1, ival2:
    * find tag in the serialized xml and get the same attribute via pointer
    */
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

    /**
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of bcm_timeval for seconds and microseconds
    * @param version: const unsigned int --> unused
    * serialize now the seconds and microseconds of bcm_timeval
    */
    template<class Archive>
    void serialize(Archive &ar, bcm_timeval &config,
                   const unsigned int version) {
        ar & boost::serialization::make_nvp("tv_sec", config.tv_sec);
        ar & boost::serialization::make_nvp("tv_usec", config.tv_usec);

    }

    /**
    * method: load_construct_data --> deserialize CANConnectorSendOperation
    * @param ar: address of an archive to deserialize
    * @param instance: pointer of a CANConnectorSendOperation object to deserialize
    * @param file_version: constant unsigned int --> unused
    *
    * @param _canID, _isCANFD, _isCyclic, _announce, _countIval1, _ival1, _ival2:
    * create helping attributes for deserializing
    * deserialize now the helping attributes of the CANConnectorSendOperation object
    *
    * if the attribute _canID is an hex-value
    * @param ss: stringstream for handling hex-values
    * convert the string into hex value and put it into the stringstream
    * put stringstream into _canID
    * else
    * @param ss: stringstream to put it into _canID
    * write the string into the stringstream
    * put stringstream into _canID
    *
    * overwrite the current object from class with the helping variables
    */

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

    /**
    * Implementation the boost serialization for CANConnectorReceiveOperation
    *
    * @param Archive: Templates for any class, so that the following methods can be used by them
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of a CANConnectorReceiveOperation object
    * @param version: const unsigned int--> unused
    *  --> method is empty
    */

    template<class Archive>
    void serialize(Archive &ar, sim_interface::dut_connector::can::CANConnectorReceiveOperation &config,
                   const unsigned int version) {}


    /**
    * method: save_construct_data --> serialize CANConnectorReceiveOperation
    * @param ar: address of an archive to deserialize
    * @param instance: pointer of a CANConnectorReceiveOperation object to serialize
    * @param version: constant unsigned int --> unused
    * serialize now the attributes of the CANConnectorReceiveOperation object:
    * @param operation, isCANFD, hasMask, maskLength, mask:
    * find tag in the serialized xml and get the same attribute via pointer
    */

    template<class Archive>
    inline void
    save_construct_data(Archive &ar, const sim_interface::dut_connector::can::CANConnectorReceiveOperation *config,
                        const unsigned int version) {

        ar & boost::serialization::make_nvp("operation", config->operation);
        ar & boost::serialization::make_nvp("isCANFD", config->isCANFD);
        ar & boost::serialization::make_nvp("hasMask", config->hasMask);

        std::stringstream hex;
        hex << "0x";
        hex << std::hex;
        int _maskLength = CAN_MAX_DLEN;

        if (config->isCANFD) {
            _maskLength = CANFD_MAX_DLEN;
        }
        for (int i = 0; i < _maskLength; i++) {

            hex << std::setw(2) << std::setfill('0') << (int) config->mask.data[i];
        }

        std::string stringHexValue = hex.str();
        ar & boost::serialization::make_nvp("mask", stringHexValue);

    }

    /**
    * method: load_construct_data --> deserialize CANConnectorReceiveOperation
    * @param ar: address of an archive to deserialize
    * @param instance: pointer of a CANConnectorReceiveOperation object to deserialize
    * @param file_version: constant unsigned int --> unused
    *
    * @param _operation, _isCANFD, _hasMask, _maskLength, _mask:
    * create helping attributes for serializing
    * deserialize now the helping attributes of the CANConnectorReceiveOperation object
    * overwrite the current object from class with the helping variables
    */

    template<class Archive>
    inline void
    load_construct_data(Archive &ar, sim_interface::dut_connector::can::CANConnectorReceiveOperation *instance,
                        const unsigned int file_version) {

        std::string _operation;
        bool _isCANFD;
        bool _hasMask;
        int _maskLength = CAN_MAX_DLEN;

        std::string hexMask;

        ar & boost::serialization::make_nvp("operation", _operation);
        ar & boost::serialization::make_nvp("isCANFD", _isCANFD);
        ar & boost::serialization::make_nvp("hasMask", _hasMask);
        ar & boost::serialization::make_nvp("mask", hexMask);

        __u8 _maskCANLength[CAN_MAX_DLEN] = {0};
        __u8 _maskCANFDLength[CANFD_MAX_DLEN] = {0};
        __u8 *_mask = _maskCANLength;

        if (_isCANFD) {
            _maskLength = CANFD_MAX_DLEN;
            _mask = _maskCANFDLength;
        }
        // Parse every two chars into one byte
        for (unsigned int i = 0; i < _maskLength && i < hexMask.length() - 2; i += 2) {
            std::string byteString = hexMask.substr(i + 2, 2);
            __u8 byte = (__u8) strtol(byteString.c_str(), NULL, 16);
            _mask[i / 2] = byte;
        }

        ::new(instance)sim_interface::dut_connector::can::CANConnectorReceiveOperation(_operation, _isCANFD,
                                                                                       _hasMask, _maskLength, _mask
        );
    }

    /**
    * Implementation the boost serialization for the map with canid_t and CANConnectorReceiveOperation
    * @param Archive: Templates for any class, so that the following methods can be used by them
    *
    * method: serialize
    * @param ar: address of an archive
    * @param config: address of the map with canid_t and CANConnectorReceiveOperation
    * @param unsigned --> unused
    *  --> method is empty
    */

    template<class Archive>
    void
    serialize(Archive &ar, std::map<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> &config,
              unsigned) {
    }

    /**
    * method: save_construct_data --> serialize the map with canid_t and CANConnectorReceiveOperation
    * @param ar: address of an archive to serialize
    * @param config: pointer of the map with canid_t and CANConnectorReceiveOperation to serialize
    * @param unsigned --> unused
    * @param count: contains the length of the config pointer(size of the map)
    * serialize count
    * for each loop: run through each address in the config pointer (map)
    *  @param pointer: const auto *pointer = make a pointer to a pair with the map and cast it
    *  serialize every element in the map
    */

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

    /**
    * method: load_construct_data --> deserialize the map with canid_t and CANConnectorReceiveOperation
    * @param ar: address of an archive to deserialize
    * @param config: pointer of the map with canid_t and CANConnectorReceiveOperation to deserialize
    * @param unsigned: unsigned int --> unused
    *
    * @param _pair, _count, _map:
    * create helping attributes for deserializing
    * deserialize count
    * for loop:
    *   deserialize every element in the pair
    *   insert the pair into helping map
    *
    * overwrite the current map from class with the helping variables
    */

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

    /**
    * @param Archive: Templates for any class, so that the following methods can be used by them
    *
    * Implementation the boost serialization for the pair with canid_t and CANConnectorReceiveOperation
    *
    * method: serialize
    * @param ar: address of an archive
    * @param p: address of the pair with canid_t and CANConnectorReceiveOperation
    * @param unsigned --> unused
    *  --> method is empty
    */

    template<class Archive>
    void serialize(Archive &ar, std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> &p,
                   unsigned) {

    }

    /**
    * method: save_construct_data --> serialize the pair with canid_t and CANConnectorReceiveOperation
    * @param ar: address of an archive to serialize
    * @param pair: address of the pair with canid_t and CANConnectorSendOperation to serialize
    * @param file_version --> unused
    * @param pointer: *pointer form type CANConnectorReceiveOperation and save CANConnectorReceiveOperation via *pair
    *
    * serialize operation and CANConnectorSendOperation
    */
    template<class Archive>
    inline void save_construct_data(Archive &ar,
                                    const std::pair<canid_t, sim_interface::dut_connector::can::CANConnectorReceiveOperation> *pair,
                                    const unsigned int file_version) {

        const sim_interface::dut_connector::can::CANConnectorReceiveOperation *pointer = &pair->second;
        ar & boost::serialization::make_nvp("canid", pair->first);
        ar & boost::serialization::make_nvp("CANConnectorReceiveOperation", pointer);

    }

    /**
    * method: load_construct_data --> deserialize the pair with canid_t and CANConnectorReceiveOperation
    * @param ar: address of an archive to deserialize
    * @param pair: address of the pair with canid_t and CANConnectorReceiveOperation to deserialize
    * @param file_version: unsigned int --> unused
    *
    * @param _key, *_value:
    * create helping attributes for deserializing
    * deserialize _key:canid
    * deserialize _value:CANConnectorReceiveOperation
    *
    * if the attribute canid is an hex-value
    *  @param ss: Stringstream for handling hex-values
    *  convert the string into hex value and put it into the stringstream
    *  put stringstream into canid
    * else
    *  @param ss: Stringstream to put it into canid
    *  write the string into the stringstream
    *  put stringstream into canid
    *  overwrite the pair with the helping variables
    */

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