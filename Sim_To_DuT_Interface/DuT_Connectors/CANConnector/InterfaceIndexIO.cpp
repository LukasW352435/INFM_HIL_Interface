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


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/

InterfaceIndexIO::InterfaceIndexIO(const char* interfaceName) : ifr(){
    std::strncpy(ifr.ifr_name, interfaceName, IF_NAMESIZE);
}

/**
 * Returns the POSIX ioctl() value for getting the interface index by name.
 *
 * @returns The value of SIOCGIFINDEX
 */
int InterfaceIndexIO::name(){
    return SIOCGIFINDEX;
}

/**
 * Returns the ifreq struct that contains the interface name.
 *
 * @return The ifreq with the name
 */
void* InterfaceIndexIO::data(){
    return &ifr;
}

/**
 * Returns the resolved interface index.
 *
 * @return The interface index
 */
int InterfaceIndexIO::index() const{
    return ifr.ifr_ifindex;
}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/