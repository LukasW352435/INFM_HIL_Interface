/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      InterfaceIndexIO.h
 \brief     I/O control command class for getting the interface index.
            For further information see boost asio io_control.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H
#define SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes

// System includes
#include <string>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
class InterfaceIndexIO {

public:

    /**
    * Constructor.
    *
    * @param interfaceName - The name of the interface which index should be resolved.
    */
    explicit InterfaceIndexIO(std::string interfaceName);

    /**
    * Returns the POSIX ioctl() value for getting the interface index by name.
    *
    * @returns The value of SIOCGIFINDEX
    */
    static int name();

    /**
    * Returns the ifreq struct that contains the interface name.
    *
    * @return The ifreq with the name
    */
    void *data();

    /**
    * Returns the resolved interface index.
    *
    * @return The resolved interface index.
    */
    int index() const;

private:
    ifreq ifr{}; /**< The ifreq struct used for ioctl */

};


#endif //SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
