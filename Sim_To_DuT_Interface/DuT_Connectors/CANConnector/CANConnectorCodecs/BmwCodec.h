/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.cpp
 \brief     Implements the CAN codec for the BMW.
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      02.12.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_BMWCODEC_H
#define SIM_TO_DUT_INTERFACE_BMWCODEC_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include <CANConnectorCodec.h>

// System includes


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class BmwCodec : public CANConnectorCodec{

    public:
        // Function members

        BmwCodec(){
            DuTLogger::logMessage("Build Bmw Codec", LOG_LEVEL::DEBUG, false);
        }

        canfd_frame parseEventToFrame(const SimEvent &event) override;

        // Data members

    private:
        // Function members

        // Data members

    };

}


#endif //SIM_TO_DUT_INTERFACE_BMWCODEC_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/