/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorCodecFactory.h
 \brief
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/

#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "CANConnectorCodec.h"

const static std::string CODEC_NAME_BMW = "bmwCodec";
const static std::string CODEC_NAME_SUZUKI = "suzukiCodec";


namespace sim_interface::dut_connector::can {

    class CANConnectorCodecFactory {
    public:
        static CANConnectorCodec* createCodec(const std::string &codecName);
    };
}

#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H
