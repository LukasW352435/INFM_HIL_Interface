/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      Dummy_CANFD_Socket.h
 \brief     Provides functions for the setup of an raw CAN/CANFD socket.
 \author    Matthias Bank
 \version   1.0.0
 \date      24.10.2021
 ******************************************************************************/
#ifndef DUMMY_CANFD_SOCKET_H
#define DUMMY_CANFD_SOCKET_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <linux/can.h>


/*******************************************************************************
 * FUNCTION DECLARATIONS
 ******************************************************************************/

/**
 * Creates and binds a CAN/CANFD socket on an interface.
 * Support for CANFD frames can be enabled if needed.
 *
 * @param socketFD  - Storage for the created socket descriptor
 * @param addr     -  Storage for the sockaddr_can of the socket
 */
extern int setupSocket(int *socketFD, struct sockaddr_can *addr);


#endif //DUMMY_CANFD_SOCKET_H


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
