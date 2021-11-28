/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorReceiveOperation.cpp
 \brief     Contains the necessary information to configure a receive operation.
            Used for the configuration of the CAN Connector.
 \author    Matthias Bank
 \version   1.0.0
 \date      27.11.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "CANConnectorReceiveOperation.h"


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    CANConnectorReceiveOperation::CANConnectorReceiveOperation(std::string operation, canid_t canID, bool isCANFD,
                                                               bool hasMask, __u8 *maskData, int maskLength):
                                                               operation(std::move(operation)), canID(canID),
                                                               isCANFD(isCANFD), hasMask(hasMask), mask(),
                                                               maskLength(maskLength){

        // Assert that:
        if(this->isCANFD){
            // - the mask length is not greater than the CANFD MTU and greater than zero
            assert(0 < this->maskLength && this->maskLength <= CANFD_MAX_DLEN);
        }else{
            // - the mask length is not greater than the CAN MTU and greater than zero
            assert(0 < this->maskLength && this->maskLength <= CAN_MAX_DLEN);
        }

        // Check if we have a mask
        if(this->hasMask){

            // Assert that the mask data is not a null pointer
            assert(maskData != nullptr);

            // Copy the mask data
            std::memcpy(reinterpret_cast<void *>(this->mask[0]), maskData, maskLength);
        }

    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/