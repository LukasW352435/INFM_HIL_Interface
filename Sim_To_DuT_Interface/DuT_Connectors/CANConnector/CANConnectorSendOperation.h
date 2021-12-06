/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorSendOperation.h
 \brief     Contains the necessary information to configure a send operation.
            Used for the configuration of the CAN Connector.
 \author    Matthias Bank
 \version   1.0.0
 \date      27.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes

// System includes
#include <string>
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>

/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnectorSendOperation{

    public:
        // Functions members

        /**
         * Constructor
         *
         * @param canID     - The CAN ID of the frame.
         * @param isCANFD   - Flag for CANFD frames.
         * @param isCyclic  - Flag for cyclic frames.
         * @param announce  - Flag for immediately sending out updates once.
         * @param count     - Number of times the frame is send with the first interval.
         * @param ival1     - First Interval.
         * @param ival2     - Second Interval.
         */
        CANConnectorSendOperation(canid_t canID,
                                  bool isCANFD,
                                  bool isCyclic,
                                  bool announce = false,
                                  __u32 count = 0,
                                  struct bcm_timeval ival1 = {0},
                                  struct bcm_timeval ival2 = {0});

        // Data members
        canid_t canID;                  /**< The CAN ID of the frame.                                   */
        bool isCANFD;                   /**< Flag for CANFD frames.                                     */
        bool isCyclic;                  /**< Flag for cyclic messages.                                  */
        bool announce;                  /**< Flag for immediately sending out updates once.             */
        __u32 count;                    /**< Number of times the frame is send with the first interval. */
        struct bcm_timeval ival1;       /**< First Interval.                                            */
        struct bcm_timeval ival2;       /**< Second Interval.                                           */
    CANConnectorSendOperation() = default;
    private:
        // Functions members

        // Data members

         friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & archive, const unsigned int version){
            archive & BOOST_SERIALIZATION_NVP(canID);
            archive & BOOST_SERIALIZATION_NVP(isCANFD);
            archive & BOOST_SERIALIZATION_NVP(isCyclic);
            archive & BOOST_SERIALIZATION_NVP(announce);
            archive &  BOOST_SERIALIZATION_NVP(count);
          archive & BOOST_SERIALIZATION_NVP(ival1.tv_sec);
            archive & BOOST_SERIALIZATION_NVP(ival1.tv_usec);
          archive & BOOST_SERIALIZATION_NVP(ival2.tv_sec);
           archive & BOOST_SERIALIZATION_NVP(ival2.tv_usec);






        }
        template<class Archive>
        inline void load_construct_data(Archive & archive, CANConnectorSendOperation * configPtr, const unsigned int version)
        {   canid_t _canID;
            bool _isCANFD;
            bool _isCyclic;
            bool _announce;
            __u32 _count;
            struct bcm_timeval _ival1 ;
            struct bcm_timeval _ival2;



            archive >> BOOST_SERIALIZATION_NVP(_canID);
            archive >>BOOST_SERIALIZATION_NVP (_isCANFD);
            archive >> BOOST_SERIALIZATION_NVP(_isCyclic);
            archive >> BOOST_SERIALIZATION_NVP(_announce);
            archive >> BOOST_SERIALIZATION_NVP(_count);
            archive >>BOOST_SERIALIZATION_NVP( _ival1);
            archive >>BOOST_SERIALIZATION_NVP( _ival2);



            ::new (configPtr) CANConnectorSendOperation ( _canID, _isCANFD,  _isCyclic,
                    _announce, _count,  _ival1,_ival2 );

        }


    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORSENDOPERATION_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/