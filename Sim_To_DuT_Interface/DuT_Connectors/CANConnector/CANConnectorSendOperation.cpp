/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorSendOperation.cpp
 \brief     Contains the necessary information to configure a send operation.
            Used for the configuration of the CAN Connector.
 \author    Matthias Bank
 \version   1.0.0
 \date      27.11.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "CANConnectorSendOperation.h"


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    CANConnectorSendOperation::CANConnectorSendOperation(canid_t canID,
                                                         bool isCANFD,
                                                         bool isCyclic,
                                                         bool announce,
                                                         __u32 count,
                                                         struct bcm_timeval ival1,
                                                         struct bcm_timeval ival2):
                                                         canID(canID),
                                                         isCANFD(isCANFD),
                                                         isCyclic(isCyclic),
                                                         announce(announce),
                                                         count(count),
                                                         ival1(ival1),
                                                         ival2(ival2){

        // Check if it is a cyclic operation
        if(this->isCyclic){

            // Assert that the elements of ival1 and ival2 are not negative
            if(this->ival1.tv_sec < 0 || this->ival1.tv_usec < 0 || this->ival2.tv_sec < 0 || this->ival2.tv_usec < 0){
                throw std::invalid_argument("CAN Connector Send Operation: ival value must not be negative");
            }

            // Assert: that at least one value of ival2 is greater than zero
            if(this->count == 0){

                // Note: If count is zero only ival2 is being used
                // - that at least one value of ival2 is greater than zero
                if(this->ival2.tv_sec == 0 && this->ival2.tv_usec == 0){
                    throw std::invalid_argument("CAN Connector Send Operation: ival must contain at least one value greater than zero");
                }

            }else{

                // - that at least one value of each ival is greater than zero
                if( (this->ival1.tv_sec == 0 && this->ival1.tv_usec == 0) || (this->ival2.tv_sec == 0 && this->ival2.tv_usec == 0) ){
                    throw std::invalid_argument("CAN Connector Send Operation: ival must contain at least one value greater than zero");
                }

            }

        }

    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/