# V2X Connector

This connector provides an interface between a network interface for V2X and this interface.

A raw socket is opened on the configured interface and the packets received are sent as events to the simulation.
Similarly, received V2X events are sent out over the socket.

> ⚠ The raw socket requires root privileges to be opened, alternatively the CAP_NET_RAW capability needs to be given to the executable

## Requirements
- root or CAP_NET_RAW capability, can be set via `sudo setcap cap_net_raw=ep executable`

## Config
- **ifname** network interface to use for sending and receiving 
- **ethernetFrameType** two bytes for the indication of frameType after the source and destination MAC in the ethernet header
- **operations**, **periodicOperations**, **periodicTimerEnabled** from [Base Connector](../ReadMe.md) are not used (fixed operation of "V2X", no periodic sending)

## Testing
To test the V2X Connector two virtual interfaces can be created and linked together with the following commands.
```shell
sudo ip link add veth0 type veth peer name veth1
sudo ip link set veth0 up
sudo ip link set veth1 up
```

Packets send to veth0 are received on veth1 and vice versa.