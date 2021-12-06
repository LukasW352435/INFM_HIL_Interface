/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorCodecFactory.h
 \brief     Creates the requested CAN codec.
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "CANConnectorCodecFactory.h"

// Codecs includes
#include "BmwCodec.h"
#include "SuzukiCodec.h"


/*******************************************************************************
 * DEFINES
 ******************************************************************************/

/**
 * The name of the BMW CAN codec.
 */
#define CODEC_NAME_BMW "BmwCodec"

/**
 * The name of the Suzuki CAN codec.
 */
#define CODEC_NAME_SUZUKI "SuzukiCodec"


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    CANConnectorCodec* CANConnectorCodecFactory::createCodec(const std::string &codecName){

        // Check if the codec name is empty
        if(codecName.empty()){
            DuTLogger::logMessage("CAN Connector: Codec name can not be empty", LOG_LEVEL::ERROR);
            throw std::invalid_argument("CAN Connector: Codec name can not be empty");
        }

        // Create the right codec with the given codec name
        if(CODEC_NAME_BMW == codecName){
            return new BmwCodec();
        }else if(CODEC_NAME_SUZUKI == codecName){
            return new SuzukiCodec();
        }else{
            // Unknown CAN codec name
            DuTLogger::logMessage("CAN Connector: Unknown CAN codec name <" + codecName + ">. Could not create the CAN codec", LOG_LEVEL::ERROR);
            throw std::invalid_argument("CAN Connector: Unknown CAN codec name <" + codecName + ">. Could not create the CAN codec");
        }

    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/