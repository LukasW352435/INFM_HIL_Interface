#include <iostream>
#include "SimToDuTInterface.h"
#include <thread>
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConfig.h"
#include "Utility/SharedQueue.h"

int main() {
    // Create interface
    SimToDuTInterface interface;
    // Create simComHandler
    SimComHandler simComHandler(interface.getQueueSimToInterface());
    //interface.setSimComHandler(&simComHandler);

    // Create DuT Devices
    thi::dut_connector::rest_dummy::RESTConfig config;
    config.baseUrlDuT = "http://localhost:9090";
    config.baseCallbackUrl = "http://172.17.0.1";
    config.port = 9091;

    thi::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(), config);
    auto event = SimEvent();
    event.operation = "Left Abc";
    event.value = "xyz";
    restDummyConnector.handleEvent(event);
    DuTConnector duTConnector2(interface.getQueueDuTToSim());
    interface.addConnector(&restDummyConnector);
    interface.addConnector(&duTConnector2);

    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    simComHandler.run();

    // Start interface to receive/send events
    interface.run();

    std::string s;
    std::cin >> s;
    return 0;
}
