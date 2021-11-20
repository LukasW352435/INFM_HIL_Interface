#include <iostream>
#include "SimToDuTInterface.h"
#include <thread>
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConfig.h"
#include "Utility/SharedQueue.h"
#include "Sim_Communication/SimComHandler.h"
#include "DuTLogger/DuTLogger.h"
#include "DuT_Connectors/V2XConnector/V2XConfig.h"
#include "DuT_Connectors/V2XConnector/V2XConnector.h"

int main() {
//    DuTLogger::logMessage("Start Application", LOG_LEVEL::INFO);
    
    // Create interface
    sim_interface::SimToDuTInterface interface;
    // Create simComHandler
    /*
    std::string socketSimAddress = "tcp://localhost:7777";
    zmq::context_t context_sub(1);
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface(), socketSimAddress, context_sub);
*/
    //interface.setSimComHandler(&simComHandler);


    // Create DuT Devices
    sim_interface::dut_connector::rest_dummy::RESTConfig config;
    config.baseUrlDuT = "http://localhost:9090";
    config.baseCallbackUrl = "http://172.17.0.1";
    config.port = 9091;
    config.operations = {"Left Abc"};

 //   std::string Test = simComHandler.getMessageFromSim();
    
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

    //V2x Connector
    sim_interface::dut_connector::v2x::V2XConfig v2xconfig;
    v2xconfig.params.multicastIp = "239.67.77.67";
    v2xconfig.params.multicastPort = 33211;
    v2xconfig.params.ccuIp = "172.16.2.1";
    v2xconfig.params.ccuPort = 33210;



    sim_interface::dut_connector::v2x::V2XConnector v2xConnector(interface.getQueueDuTToSim(), v2xconfig);

    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
   // simComHandler.run();

    // Start interface to receive/send events
    interface.run();
    
    std::string s;
    std::cin >> s;
    
//    DuTLogger::logMessage("Shut down application", LOG_LEVEL::INFO);
    return 0;
}
