# INFM HIL Interface
Interface between the Artery V2X Simulation Framework and DuT (Device under Test).  
The interface supports standards like:

- CAN/CANFD
- V2X
- REST (for debugging / monitoring purposes)

But most importantly, more standards like I2C or 802.11p can be added later on.

Logging of the communication is included in the interface.
Aside from normal logging, structured logging of events is included as well.


## Useful links
- [Doxygen documentation of INFM_HIL_Interface](https://lukasw352435.github.io/INFM_HIL_Interface)
- [Artery simulation](https://github.com/riebl/artery)

### Dummy DuTs
- [CAN Dummy DuT](Dummy_DuT/Dummy_CAN_DuT/README.md)
- [REST Dummy DuT](Dummy_DuT/REST_Dummy_DuT/ReadMe.md)

### Connectors
- [Base Connector](Sim_To_DuT_Interface/DuT_Connectors/ReadMe.md)
- [CAN Connector](Sim_To_DuT_Interface/DuT_Connectors/CANConnector/ReadMe.md)
- [REST Dummy Connector](Sim_To_DuT_Interface/DuT_Connectors/RESTDummyConnector/ReadMe.md)
- [V2X Connector](Sim_To_DuT_Interface/DuT_Connectors/V2XConnector/ReadMe.md)

## Clone
Clone the repository with submodules (but not all, as you dont want to compile openssl from scratch)
```bash
git clone https://github.com/LukasW352435/INFM_HIL_Interface.git && cd INFM_HIL_Interface && git submodule init && git submodule update --depth 1
# or via ssh
git clone git@github.com:LukasW352435/INFM_HIL_Interface.git && cd INFM_HIL_Interface && git submodule init && git submodule update --depth 1
```

## Requirements for building

- libasio-dev
- libssl-dev
- libcurl4-openssl-dev
- restbed (is built from source)
- libzmq3-dev (ZeroMQ)
- quill (is built from source)
