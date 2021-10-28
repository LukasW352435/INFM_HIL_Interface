# Dummy_CAN_DuT

## Overview
This dummy application is a simple "echo server" that sends the received CAN/CANFD frames back with a different ID. 
The dummy keeps running till it is terminated with "CTRL + C".

## Configuration
The following parameters can be changed in the Dummy_CANFD_Config.h file:
| Parameter   | Description                                                  |
| ----------- | -------------------------------------------------------------|
| FRAMEID     | The frame ID that the dummy is using to send the frames back |
| CANFD       | Enables or disables the support for CANFD                    |
| INTERFACE   | The name of the interface that should be used                |
| VERBOSE     | Enables output to the console during the receive-send loop   |

## Example
To test the dummy you can use a virtual CAN interface:

```
// Create a virtual CAN interface
ip link add dev vcan0 type vcan

// Bring the interface up and running
ip link set vcan0 up

// Start the dummy application

// Send a frame on the virtual CAN interface
cansend vcan0 123#DEADBEEF
```