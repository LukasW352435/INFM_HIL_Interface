/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      BmwCodec.cpp
 \brief     Implements some utilities for the codec
 \author    Matthias Bank, Marco Keul
 \version   1.0.0
 \date      03.12.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CODECUTILITIES_H
#define SIM_TO_DUT_INTERFACE_CODECUTILITIES_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes

// System includes
#include <netinet/in.h>
#include <cstring>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CodecUtilities{

    public:
        // Function members

        /**
        * Checks the endianness of the host.
        * True means that the host uses big endian.
        * False meas that the host uses little endian.
        *
        * @return The endianness of the system.
        */
        static bool checkEndianness();

        /**
         *  Converts the endianness of a uint16_t, uint32_t or uint64_t.
         *  Note: Since uint8_t is only one byte it doesn't need a conversion.
         *
         * @param number - The number that should be converted.
         *
         * @return The converted number.
         */
        static uint16_t convertEndianness(uint16_t number);
        static uint32_t convertEndianness(uint32_t number);
        static uint64_t convertEndianness(uint64_t number);

        // Data members

    private:
        // Function members

        // Data members


    };

}


#endif //SIM_TO_DUT_INTERFACE_CODECUTILITIES_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/