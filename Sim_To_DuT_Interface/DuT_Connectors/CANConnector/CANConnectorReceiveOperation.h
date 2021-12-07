/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorReceiveOperation.h
 \brief     Contains the necessary information to configure a receive operation.
            Used for the configuration of the CAN Connector.
 \author    Matthias Bank
 \version   1.0.0
 \date      27.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes

// System includes
#include <string>
#include <cstring>
#include <cassert>
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <iostream>
/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorReceiveOperation{

    public:
        // Functions members

        /**
         * Constructor for an receive operation.
         *
         * @param operation  - The operation name.
         * @param isCANFD    - Flag for CANFD frames.
         * @param hasMask    - Flag if a mask should be used.
         * @param maskLength - The length of the mask.
         * @param maskData   - The mask data that should be used.
         */
        CANConnectorReceiveOperation(std::string operation,
                                     bool isCANFD,
                                     bool hasMask,
                                     int maskLength = 0,
                                     __u8* maskData = nullptr);

        // Data members
        std::string operation;            /**< The operation name.            */
        bool isCANFD;                     /**< Flag for CANFD frames.         */
        bool hasMask;                     /**< Flag if a mask should be used. */
        int maskLength;                   /**< The length of the mask data.   */
        struct canfd_frame mask = {0};    /**< The mask that should be used.  */


    private:




        // Functions members

        // Data members
        /*
     friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & archive, const unsigned int version){
     //       archive & BOOST_SERIALIZATION_NVP(operation);
     //       archive & BOOST_SERIALIZATION_NVP(isCANFD);
     //       archive & BOOST_SERIALIZATION_NVP(hasMask);
     //       archive & BOOST_SERIALIZATION_NVP(maskLength);
//
     //     archive &  BOOST_SERIALIZATION_NVP(mask.can_id);
     //     archive &  BOOST_SERIALIZATION_NVP(mask.len);
     //     archive &  BOOST_SERIALIZATION_NVP(mask.flags);
     //     archive &  BOOST_SERIALIZATION_NVP(mask.__res0);
     //     archive &  BOOST_SERIALIZATION_NVP(mask.__res1);
     //     archive &  BOOST_SERIALIZATION_NVP(mask.data);

        }
        template<class Archive>
        inline void load_construct_data(Archive & archive, CANConnectorReceiveOperation * configPtr, const unsigned int version)
        {     std::string _operation;
              bool _isCANFD;
              bool _hasMask;
              int _maskLength;
                __u8* _maskData  ;

        //    archive >> BOOST_SERIALIZATION_NVP(_operation);
        //    archive >>BOOST_SERIALIZATION_NVP (_isCANFD);
        //    archive >> BOOST_SERIALIZATION_NVP(_hasMask);
        //    archive >> BOOST_SERIALIZATION_NVP(_maskLength);
        //    archive >> BOOST_SERIALIZATION_NVP(_maskData);
//

            ::new (configPtr) CANConnectorReceiveOperation ( _operation, _isCANFD,  _hasMask,
                    _maskLength, _maskData );

        }
*/
    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORRECEIVEOPERATION_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/