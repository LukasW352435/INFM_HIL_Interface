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
class InterfaceIndexIO{

public:
    explicit InterfaceIndexIO(std::string interfaceName);
    static int name();
    void* data();
    int index() const;

private:
    ifreq ifr{};

};


#endif //SIM_TO_DUT_INTERFACE_INTERFACEINDEXIO_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
