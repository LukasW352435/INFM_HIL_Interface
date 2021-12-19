/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      InterfaceIndexIO.cpp
 \brief     I/O control command class for getting the interface index.
            For further information see boost asio io_control.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "InterfaceIndexIO.h"

// System includes


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/

InterfaceIndexIO::InterfaceIndexIO(std::string interfaceName) : ifr() {
    std::strncpy(ifr.ifr_name, interfaceName.c_str(), IF_NAMESIZE);
}

int InterfaceIndexIO::name() {
    return SIOCGIFINDEX;
}

void *InterfaceIndexIO::data() {
    return &ifr;
}

int InterfaceIndexIO::index() const {
    return ifr.ifr_ifindex;
}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/