#include <iostream>
#include "SimToDuTInterface.h"
#include <thread>
#include "Sim_Communication/SimComHandler.h"
#include "DuT_Connectors/DuTInfo.h"
#include "Utility/SharedQueue.h"

int main() {
    // Create interface
    SimToDuTInterface interface;

    // Create simComHandler
    SimComHandler simComHandler(interface.getQueueSimToInterface());
    interface.setSimComHandler(&simComHandler);

    // Create DuT Devices
    DuTConnector duTConnector1(interface.getQueueDuTToSim());
    DuTConnector duTConnector2(interface.getQueueDuTToSim());
    interface.addConnector(&duTConnector1);
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
