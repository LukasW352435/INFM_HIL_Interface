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
namespace sim_interface::dut_connector::can {

    CANConnectorReceiveOperation::CANConnectorReceiveOperation(std::string operation,
                                                               bool isCANFD,
                                                               bool hasMask,
                                                               int maskLength,
                                                               __u8 *maskData) :
            operation(std::move(operation)),
            isCANFD(isCANFD),
            hasMask(hasMask),
            maskLength(maskLength) {

        // Assert that:
        if (this->hasMask) {

            // Note: We need to use CAN/CANFD_MAX_DLEN and not CAN/CANFD_MTU because the DLEN is
            // the max size of the data array and the MTU is the sizeof the struct as a whole.
            if (this->isCANFD) {

                // - the mask length is greater than zero and not greater than the CANFD MTU
                if (this->maskLength < 0 || CANFD_MAX_DLEN < this->maskLength) {
                    throw std::invalid_argument(
                            "CAN Connector Receive Operation: Mask must not be negative or exceed the CANFD frame length");
                }

            } else {

                // - the mask length is greater than zero and not greater than the CAN MTU
                if (this->maskLength < 0 || CAN_MAX_DLEN < this->maskLength) {
                    throw std::invalid_argument(
                            "CAN Connector Receive Operation: Mask must not be negative or exceed the CAN frame length");
                }

            }

        }

        // Check if we have a mask
        if (this->hasMask) {

            // Assert that the mask data is not a null pointer
            if (maskData == nullptr) {
                throw std::invalid_argument(
                        "CAN Connector Receive Operation: Mask must not be NULL for a receive operation that has a mask");
            }

            // Set the mask length and copy the mask data
            this->mask.len = maskLength;
            std::memcpy(reinterpret_cast<void *>(this->mask.data), maskData, maskLength);
        }

    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/