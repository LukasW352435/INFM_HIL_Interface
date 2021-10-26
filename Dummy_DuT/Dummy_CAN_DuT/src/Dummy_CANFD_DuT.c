/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      Dummy_CANFD_DuT.c
 \brief     This dummy application is a simple "echo server" that sends the
            received CAN/CANFD frames back with a different ID. The dummy
            keeps running till it is terminated with "CTRL + C".
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
#include <linux/can.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>


/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
volatile int keepRunning = 1; // Keep running till CTRL + F is pressed


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/

/**
 * Process termination signal
 *
 * @param signumber Signal number which occurred
 */
static void handleTerminationSignal(int signumber){

    // Stop the application
    keepRunning = 0;
}

/**
 * Handles the shutdown procedure
 *
 * @param retCode  - The return code
 * @param frame    - Storage for the generated frame
 * @param socketFD - The socket file descriptor
 */
int shutdownHandler(int retCode, void *frame, int socketFD){

    // Free the frame memory
    if(frame != NULL){
        free(frame);
    }

    // Close the socket
    if(socketFD != -1){
        close(socketFD);
    }

    exit(retCode);
}

/**
 * Prints a buffer of hex values
 *
 * @param buffer     - The buffer containing the hex values
 * @param bufferSize - The size of the buffer
 */
void printHexBuffer(unsigned char *const buffer, int bufferSize){

    for(int index = 0; index < bufferSize; index++){
        printf("%x", buffer[index]);
    }

    printf("\n");
}

int main(){

    struct sigaction sigAction;                     // Signal action for CTRL + F

    int socketFD = -1;                              // Socket file descriptor
    struct sockaddr_can socketAddr;                 // Socket address

    int numbytes;                                   // Number of bytes that we sent or received
    unsigned char buf[CANFD_MTU];                   // Buffer for storing the received message payload

    unsigned int recvID;                            // The ID of the received frame
    void *frame = NULL;                             // Frame that is sent back
    size_t frameSize;                               // The size of the frame that is being received
    int recvPayloadSize;                            // The size of the payload of the received frame


    // Process termination signal for CTRL + F
    sigAction.sa_handler = handleTerminationSignal;

    if(sigaction(SIGINT, &sigAction, NULL) < 0){
        printf("Setting signal handler for SIGINT failed \n");
        shutdownHandler(ERR_SIGACTION_FAILED, frame, socketFD);
    }

    // Set up the socket
    if(setupSocket(&socketFD, &socketAddr) != 0){
        printf("Error could not setup the socket \n");
        shutdownHandler(ERR_SETUP_FAILED, frame, socketFD);
    }

    printf("Setup the socket on the interface %s\n", INTERFACE);

    // Check which mode is selected and set values accordingly
    if(CANFD){
        frameSize = CANFD_MTU;
        frame = malloc(frameSize);
    }else{
        frameSize = CAN_MTU;
        frame = malloc(frameSize);
    }

    // Error handling
    if(frame == NULL){
        printf("Error could not allocate memory \n");
        shutdownHandler(ERR_MALLOC_FAILED, frame, socketFD);
    }

    // Send Receive Loop
    printf("\n### Starting receive-send loop: ### \n\n");

    while(keepRunning){

        if(VERBOSE){
            printf("Blocking on receive! \n");
        }

        // Receive data from the socket
        numbytes = recv(socketFD, frame, frameSize, 0);

        // Error handling:
        if(numbytes == -1){
            perror("Error recv failed");
            shutdownHandler(ERR_RECV_FAILED, frame, socketFD);
        }else if(numbytes != CAN_MTU && numbytes != CANFD_MTU){
            printf("Error received bytes are not equal to the size of a complete CAN/CANFD frame \n");
            shutdownHandler(ERR_RECV_FAILED, frame, socketFD);
        }

        // Get the ID and set the new ID
        if(CANFD){
            struct canfd_frame *canfdFrame = (struct canfd_frame*) frame;
            recvID = canfdFrame->can_id;
            canfdFrame->can_id = FRAMEID;
        }else{
            struct can_frame *canFrame = (struct can_frame*) frame;
            recvID = canFrame->can_id;
            canFrame->can_id = FRAMEID;
        }

        if(VERBOSE){

            // Get the payload and payload size
            if(CANFD){
                struct canfd_frame *canfdFrame = (struct canfd_frame*) frame;
                recvPayloadSize = canfdFrame->len;
                memcpy(buf, canfdFrame->data, canfdFrame->len);
            }else{
                struct can_frame *canFrame = (struct can_frame*) frame;
                recvPayloadSize = canFrame->can_dlc;
                memcpy(buf, canFrame->data, canFrame->can_dlc);
            }

            buf[recvPayloadSize] = '\0';
            printf("Got %d bytes total with ID 0x%x and %d bytes payload: \n", numbytes, recvID, recvPayloadSize);
            printHexBuffer(buf, recvPayloadSize);
        }

        // Send the frame
        numbytes = send(socketFD, frame, frameSize, 0);

        // Error handling:
        if(numbytes == -1){
            perror("Error sendto failed");
            shutdownHandler(ERR_SEND_FAILED, frame, socketFD);
        }else if(numbytes != CAN_MTU && numbytes != CANFD_MTU){
            printf("Error send bytes are not equal to the size of a complete CAN/CANFD frame \n");
            shutdownHandler(ERR_SEND_FAILED, frame, socketFD);
        }

        if(VERBOSE){
            printf("Sent %d bytes total with ID 0x%x and %d bytes payload: \n", numbytes, FRAMEID, recvPayloadSize);
            printHexBuffer(buf, recvPayloadSize);
        }

    }

    // Call the shutdown handler
    shutdownHandler(RET_E_OK, frame, socketFD);
    return RET_E_OK;
}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/