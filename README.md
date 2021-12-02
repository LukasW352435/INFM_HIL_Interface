# INFM HIL Interface
Interface between the Artery V2X Simulation Framework and DuT (Device under Test).  
The Interface should support standards like:

- CAN
- 802.11p
- V2X
- I2C

But most importantly, more standards can be added later on.

Logging of the communication is included in the interface.


## Useful links
- [Doxygen documentation](https://lukasw352435.github.io/INFM_HIL_Interface)
- [Artery simulation](https://github.com/riebl/artery)

# Clone
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
