/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      Dummy_CANFD_Socket.c
 \brief     Provides functions for the setup of an raw CAN/CANFD socket.
 \author    Matthias Bank
 \version   1.0.0
 \date      24.10.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "Dummy_CANFD_Error.h"
#include "Dummy_CANFD_Config.h"
#include "Dummy_CANFD_Socket.h"
#include <errno.h>
#include <linux/can/raw.h>
#include <linux/can.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/

int setupSocket(int *const socketFD, struct sockaddr_can *const addr){

    // Get the socket file descriptor for ioctl
    *socketFD = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // Error handling
    if(*socketFD == -1){
        perror("Error getting socket file descriptor failed");
        return ERR_SOCKET_FAILED;
    }

    // Set the interface name in the ifr
    struct ifreq ifr;
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", INTERFACE);

    // Get the ifrindex of the interface name
    if(ioctl(*socketFD, SIOCGIFINDEX, &ifr) < 0){
        printf("Error could not get ifrindex: %s\n", strerror(errno));
        return ERR_IF_NOT_FOUND;
    }

    // Fill in the family and ifrindex
    addr->can_family  = AF_CAN;
    addr->can_ifindex = ifr.ifr_ifindex;

    // Bind to the socket
    if(bind(*socketFD, (struct sockaddr *) addr, sizeof(struct sockaddr_can)) != 0){
        perror("Error could not bind to the socket");
        close(*socketFD);
        return ERR_SETUP_FAILED;
    }

    // Check if CANFD frame support should be enabled
    if(CANFD){

        int can_raw_fd_frames = 1;
        if(setsockopt(*socketFD, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &can_raw_fd_frames, sizeof(can_raw_fd_frames)) != 0){
            perror("Error setsockopt CAN_RAW_FD_FRAMES failed");
            close(*socketFD);
            return ERR_SETSOCKOPT_FAILED;
        }

        printf("Enabled CANFD frames support on the socket\n");
    }

    return RET_E_OK;
}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
