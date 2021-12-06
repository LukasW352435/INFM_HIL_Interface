/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorConfig.h
 \brief     The Config for the CANConnector.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "../ConnectorConfig.h"
#include "CANConnectorReceiveOperation.h"
#include "CANConnectorSendOperation.h"

// System includes
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>

/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorConfig : public ConnectorConfig{

    public:
        // Function members

        /**
         * Constructor for the CAN Connector configuration.
         *
         * @param interfaceName        - The name of the interface that should be used by the connector.
         * @param operations           - The operations supported by the CAN Connector.
         * @param codecName            - The name of the codec that should be used by the connector.
         * @param frameToOperation     - The configurations for the receive operations.
         * @param operationToFrame     - The configurations for the send operations.
         * @param periodicOperations   - The periodic operations supported by the connector through the interface.
         * @param periodicTimerEnabled - Flag that enables the periodic sending through the interface.
         *                               Since the BCM socket supports cyclic operations this should always be false
         */
        explicit CANConnectorConfig(std::string interfaceName,
                                    std::string codecName,
                                    std::set<std::string> operations,
                                    std::map<canid_t, CANConnectorReceiveOperation> frameToOperation = {},
                                    std::map<std::string, CANConnectorSendOperation> operationToFrame = {},
                                    std::map<std::string, int> periodicOperations = {},
                                    bool periodicTimerEnabled = false);

        // Data member
        std::string interfaceName;                                         /**< The name of the interface that should be used.*/
        std::string codecName;                                             /**< The name of the codec that should be used.    */
        std::map<canid_t, CANConnectorReceiveOperation>  frameToOperation; /**< Used for mapping a CAN frame to an operation. */
        std::map<std::string, CANConnectorSendOperation> operationToFrame; /**< Used for mapping an operation to a CAN frame. */

        CANConnectorConfig(): ConnectorConfig(){};
    private:
        // Function members

        // Data member

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & archive, const unsigned int version){
           // std::cout << "WTFFFFF" << std::endl;
            archive & BOOST_SERIALIZATION_NVP(interfaceName);
            archive & BOOST_SERIALIZATION_NVP(codecName);
            archive & BOOST_SERIALIZATION_NVP(operations);
            archive & BOOST_SERIALIZATION_NVP(frameToOperation);
            archive & BOOST_SERIALIZATION_NVP(operationToFrame);
            archive &  BOOST_SERIALIZATION_NVP(periodicOperations);
            archive & BOOST_SERIALIZATION_NVP(periodicTimerEnabled);






        }
        template<class Archive>
        inline void load_construct_data(Archive & archive, CANConnectorConfig * configPtr, const unsigned int version)
        {   std::string _interfaceName;
           std::string _codecName;
             std::set<std::string> _operations;
             std::map<canid_t, CANConnectorReceiveOperation>  _frameToOperation;
              std::map<std::string, CANConnectorSendOperation> _operationToFrame;
               std::map<std::string, int> _periodicOperations;
               bool _periodicTimerEnabled;



            archive >> BOOST_SERIALIZATION_NVP(_interfaceName);
             archive >> BOOST_SERIALIZATION_NVP(_codecName);
            archive >>BOOST_SERIALIZATION_NVP (_operations);
            archive >> BOOST_SERIALIZATION_NVP(_frameToOperation);
            archive >> BOOST_SERIALIZATION_NVP(_operationToFrame);
            archive >> BOOST_SERIALIZATION_NVP(_periodicOperations);
            archive >>BOOST_SERIALIZATION_NVP( _periodicTimerEnabled);



            ::new (configPtr) CANConnectorConfig ( _interfaceName, _codecName,_operations,  _frameToOperation,
                    _operationToFrame, _periodicOperations,  _periodicTimerEnabled );

        }


    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCONFIG_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
