#include <iostream>
#include "SimToDuTInterface.h"
#include <thread>
#include "DuT_Connectors/RESTDummyConnector/RESTDummyConnector.h"
#include "DuT_Connectors/RESTDummyConnector/RESTConfig.h"
#include "Utility/SharedQueue.h"
#include "Sim_Communication/SimComHandler.h"
int main() {
    // Create interface
    sim_interface::SimToDuTInterface interface;
    // Create simComHandler
    std::string socketSimAddressSub = "tcp://localhost:7777";
    zmq::context_t context_sub(1);
    std::string socketSimAddressPub = "tcp://*:8888";
    zmq::context_t context_pub(1);
    sim_interface::SimComHandler simComHandler(interface.getQueueSimToInterface(), socketSimAddressSub, context_sub, socketSimAddressPub , context_pub);

    interface.setSimComHandler(&simComHandler);

    // Create DuT Devices
    sim_interface::dut_connector::rest_dummy::RESTConfig config;
    config.baseUrlDuT = "http://localhost:9090";
    config.baseCallbackUrl = "http://172.17.0.1";
    config.port = 9091;
    config.operations = {"Test","Angle",
    "Acceleration",
    "Decel",
    "Distance",
    "Height",
    "LaneID",
    "LaneIndex",
    "LanePosition",
    "Length",
    "Position_X-Coordinate",
    "Position_Y-Coordinate",
    "Position_Z-Coordinate",
    "RoadID",
    "RouteIndex",
    "Signals",
    "Speed",
    "Width",
    "current",
    "origin"};


    sim_interface::dut_connector::rest_dummy::RESTDummyConnector restDummyConnector(interface.getQueueDuTToSim(), config);
    /*
    auto event = sim_interface::SimEvent();
    event.operation = "Test";
    event.value = Test;
    restDummyConnector.handleEvent(event);
    auto event2 = sim_interface::SimEvent();
    event.operation = "Indicator Right";
    event.value = "xyz";
    restDummyConnector.handleEvent(event);
*/
    interface.addConnector(&restDummyConnector);

    std::cout << interface << std::endl;

    // Start simComHandler to receive events from the simulation
    std::thread simComHandlerThread (&sim_interface::SimComHandler::run, &simComHandler);
    simComHandlerThread.detach();



    // Start interface to receive/send events
    std::cout  << "Nnach thread" << std::endl;
    interface.run();

    std::string s;
    std::cin >> s;
    return 0;
}
