/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorCodecFactory.h
 \brief
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "CANConnectorCodecFactory.h"
#include "BmwCodec.h"
#include "SuzukiCodec.h"
#include "DuTLogger.h"

namespace sim_interface::dut_connector::can {

    CANConnectorCodec* CANConnectorCodecFactory::createCodec(const std::string &codecName) {
        // argument check
        if (codecName.empty()) {
            DuTLogger::logMessage("Empty codec name appeared! Can't create CAN codec", LOG_LEVEL::ERROR);
            throw std::invalid_argument("The codec name is not allowed to be empty. Please use a valid name!");
        }

        // create the right codec with the given codec name
        if (CODEC_NAME_BMW == codecName) {
            return new BmwCodec();
        } else if (CODEC_NAME_SUZUKI == codecName) {
            return new SuzukiCodec();
        } else {
            // there is no codec implementation for this codec name -> throw an exception
            DuTLogger::logMessage("Unknown CAN codec name <" + codecName +
                                            "> appeared! Can't create CAN codec", LOG_LEVEL::ERROR);
            throw std::invalid_argument("Unknown CAN codec name <" + codecName + "> appeared! Can't create CAN codec");
        }
    }
}