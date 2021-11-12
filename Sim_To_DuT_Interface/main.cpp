#include <iostream>
#include "SimToDuTInterface.h"
#include <thread>
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConfig.h"
#include "Utility/SharedQueue.h"

int main() {
    // Create interface
    sim_interface::SimToDuTInterface interface;
    // Create simComHandler
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface());
    //interface.setSimComHandler(&simComHandler);

    // Create DuT Devices
    sim_interface::dut_connector::rest_dummy::RESTConfig config;
    config.baseUrlDuT = "http://localhost:9090";
    config.baseCallbackUrl = "http://172.17.0.1";
    config.port = 9091;
    config.operations = {"Left Abc"};

    sim_interface::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(), config);
    auto event = sim_interface::SimEvent();
    event.operation = "Left Abc";
    event.value = "xyz";
    restDummyConnector.handleEvent(event);
    auto event2 = sim_interface::SimEvent();
    event.operation = "Left Abc2";
    event.value = "xyz";
    restDummyConnector.handleEvent(event);
    interface.addConnector(&restDummyConnector);

    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    simComHandler.run();

    // Start interface to receive/send events
    interface.run();

    std::string s;
    std::cin >> s;
    return 0;
}
