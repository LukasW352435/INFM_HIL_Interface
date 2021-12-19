# REST Dummy Connector

This connector provides an interface between the [REST Dummy DuT](../../../Dummy_DuT/REST_Dummy_DuT/ReadMe.md) and this
interface.

A REST endpoint is started on the configured port and registered with the DuT to receive and send messages.

## Requirements

- REST_Dummy_DuT
- restbed
- libcurl

## Config

- **baseUrlDuT** to this http address the received events are sent, gets prepended with `/send` or `/read` for
  sending/receiving events
- **baseCallbackUrl** URL that the DuT should call to send events to this interface (e.g. http://localhost), gets
  prepended with `:{port}/rest/DuT`
- **port** port the baseCallbackUrl listens on
- **operations**, **periodicOperations**, **periodicTimerEnabled** see [Base Connector](../ReadMe.md)

