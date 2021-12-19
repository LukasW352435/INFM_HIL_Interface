# CAN Connector
The CAN Connector enables the communication between the simulation and a device under test (DuT) over CAN/CANFD. It
supports one-time sending of frames as well as a cyclic sending of frames. It can be configured which frames should be
received on the CAN/CANFD interface and it can also filter for content changes in the frames. The CAN Connector is not 
responsible for the configuration of the used CAN/CANFD interface (Bitrate, Sample point etc.) as this is dependent on 
the used hardware. The user must therefore ensure that the interface is configured correctly for the communication with 
the DuT. The CAN Connector builds upon the Broadcast Manager (BCM) socket of the Linux Kernel Module SocketCAN and 
the Boost.Asio network library as well as the INFM_HIL_Interface.

## Dependencies
- SocketCAN
- Boost.Asio

## Configuration
The CAN Connector is configured through a XML configuration file which contains the following options. It is also
recommended to the read the BCM socket documentation to fully understand the details of the
options [SocketCAN documentation](https://www.kernel.org/doc/Documentation/networking/can.txt).

| Parameter            | Description                                                                                             |
| ---------------------|---------------------------------------------------------------------------------------------------------|
| interfaceName        | The name of the interface that should be used.                                                          |
| operations           | See the [Base Connector documentation](../ReadMe.md).                                                   |
| codecName            | The name of the codec that should be used by the connector. See the Codec section down below.           |
| frameToOperation     | The configurations for the receive operations defined in the XML.                                       |
| operationToFrame     | The configurations for the send operations defined in the XML.                                          |
| periodicOperations   | Since the CAN Connector is handling the cyclic sending of frames itself this should always be empty.    |
| periodicTimerEnabled | Since the CAN Connector is handling the cyclic sending of frames itself this should always be false.    |

- The structure of the CAN Connector Config is described here [CANConnectorConfig](https://lukasw352435.github.io/INFM_HIL_Interface/classsim__interface_1_1dut__connector_1_1can_1_1CANConnectorConfig.html).

- The structure of a send operation is described here [CANConnectorSendOperation](https://lukasw352435.github.io/INFM_HIL_Interface/classsim__interface_1_1dut__connector_1_1can_1_1CANConnectorSendOperation.html).

- The structure of a receive operation is described here [CANConnectorReceiveOperation](https://lukasw352435.github.io/INFM_HIL_Interface/classsim__interface_1_1dut__connector_1_1can_1_1CANConnectorReceiveOperation.html).

## Codec
Since the translation of simulation messages (SimEvents) and CAN/CANFD frames is complex and is also dependend on the
DuT, this cannot be practical specified in the XML configuration file. The user must therfore implement a concret Codec
class that handles this translation task and add it as an option in the Codec factory.

- The Codec interface is described here [CANConnectorCodec](https://lukasw352435.github.io/INFM_HIL_Interface/classsim__interface_1_1dut__connector_1_1can_1_1CANConnectorCodec.html).

- The Codec factory is described here [CANConnectorCodecFactory](https://lukasw352435.github.io/INFM_HIL_Interface/classsim__interface_1_1dut__connector_1_1can_1_1CANConnectorCodecFactory.html).

## Testing
To test the CAN Connector you can use a virtual CAN interface. Ensure that you have the needed SocketCAN Kernel modules.

```
// Install the can-utils. 
sudo apt-get install can-utils

// Modprobe is needed in the case the vcan module is not loaded.
modprobe vcan

// Create a virtual CAN interface.
ip link add dev vcan0 type vcan

// Set the interface up and running.
ip link set vcan0 up

// To see the CAN/CANFD messages that are send by the CAN Connector on 
// the virtual CAN interface open a terminal and use the candump tool.
candump vcan0

// Compile and start the project

// If you want to send CAN/CANFD messages to the CAN Connector over the 
// virtual CAN interface open a terminal and use the cansend tool.
cansend vcan0 123#DEADBEEF
```

## Helpful links:
- [Base Connector](https://lukasw352435.github.io/INFM_HIL_Interface/md_Sim_To_DuT_Interface_DuT_Connectors_ReadMe.html)
- [SocketCAN documentation](https://www.kernel.org/doc/Documentation/networking/can.txt)
- [Boost.Asio documentation](https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio.html)
- [SocketCAN userspace utilities and tools](https://github.com/linux-can/can-utils)