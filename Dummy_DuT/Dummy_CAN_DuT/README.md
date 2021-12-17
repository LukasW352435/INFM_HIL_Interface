# Dummy_CAN_DuT
This dummy application is a simple "echo server" that builds upon SocketCAN. 
It sends received CAN/CANFD frames back with a different CAN ID. 
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
// Install the can-utils 
sudo apt-get install can-utils

// Modprobe is needed in the case the vcan module is not loaded.
modprobe vcan

// Create a virtual CAN interface
ip link add dev vcan0 type vcan

// Set the interface up and running
ip link set vcan0 up

// Compile the project
cmake .
make 

// Start the dummy application
./Dummy_CAN_DuT

// Open another terminal and send a frame on the virtual CAN interface
cansend vcan0 123#DEADBEEF
```

## Documentation
For further information look at the [documentation](https://mab0189.github.io/Dummy_CAN_DuT/index.html).

## Helpful links
- [SocketCAN documentation](https://www.kernel.org/doc/Documentation/networking/can.txt)
- [SocketCAN userspace utilities and tools](https://github.com/linux-can/can-utils)