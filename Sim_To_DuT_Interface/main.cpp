#include <iostream>
#include "Sim_To_DuT_Interface.h"
#include "DuT_Connectors/DuT_Connector.h"
#include "DuT_Connectors/DuT_Info.h"
#include "DuTLogger/DuTLogger.h"

int main() {

    DuTLogger::logMessage("Test for Logging", LOG_LEVEL::INFO);

    Sim_To_DuT_Interface interface;
    // TODO Init Sim_Com_Handler
    // TODO Read Config and Create Connectors
    DuT_Connector duTConnector;
    DuT_Info info = duTConnector.get_DuT_Info();

    // add DuT to the interface
    interface.add_Connector(&duTConnector);
    std::cout << interface << std::endl;

    return 0;
}
