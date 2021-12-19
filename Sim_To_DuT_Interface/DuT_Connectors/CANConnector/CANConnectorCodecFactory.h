/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnectorCodecFactory.h
 \brief     Creates the requested CAN codec.
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "CANConnectorCodec.h"
#include "InterfaceLogger.h"

// System includes


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can {

    class CANConnectorCodecFactory {

    public:
        // Function members

        /**
        * Creates the requested CAN codec.
        *
        * @param codecName - The name of the codec that should be created.
        *
        * @return The CAN codec.
        */
        static CANConnectorCodec *createCodec(const std::string &codecName);

        // Data members

    private:
        // Function members

        // Data members

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTORCODECFACTORY_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/