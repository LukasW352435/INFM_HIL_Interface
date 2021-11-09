# INFM HIL Interface
Interface between the Artery V2X Simulation Framework and DUT (Device under Test).  
The Interface should support standards like:

- CAN
- 802.11p
- V2X
- I2C

But most importantly, more standards can be added later on.

Logging of the communication is included in the interface.

## Useful links

[Artery simulation](https://github.com/riebl/artery)

## Requirements for building

- libasio-dev
- libssl-dev
- libcurl (present on most unix systems)
- restbed (is built from source)
