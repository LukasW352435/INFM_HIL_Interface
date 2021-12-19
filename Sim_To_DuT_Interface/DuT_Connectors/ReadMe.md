# DuT Connector

This connector provides the basic interface each connector must implement.

To develop a new one, simply take the class DuTConnector as your (public) base class and implement the
functions `getConnectorInfo()` and `handleEventSingle(const SimEvent &simEvent)`.

## Config

- **operations** vector of strings each representing one supported operation (all other events are ignored)
- **periodicOperations** map of operations that should be periodically repeated, value is the interval in milliseconds
- **periodicTimerEnabled** flag to enable a timer to send events periodically to the DuTs, can be disabled if the
  connector / socket supports this natively



