/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
 *
 * This file is part of "Sim To DuT Interface".
 *
 * "Sim To DuT Interface" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sim To DuT Interface" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Lukas Wagenlehner
 * @author Michael Schmitz
 * @author Franziska Ihrler
 * @author Matthias Bank
 * @author Fabian Andre Genes
 * @author Thanaancheyan Thavapalan
 * @author Marco Keul
 * @version 1.0
 */

// Project includes
#include "SimToDuTInterface.h"
#include "Sim_Communication/SimComHandler.h"
#include "Interface_Logger/InterfaceLogger.h"
#include "SystemConfig.h"

// System includes
#include <iostream>
#include <memory>

/**
* main-method:
* Create an object from SystemConfig for configuring the interface e.g. ports of the sockets
* get path from SystemConfig.xml and load configurations
*
* Create logger object and start logger
* Create interface object
*
* Create SimComHandler object and set object to interface
* Get serialized xml-config file from simulation
*
* Start SimComHandler to receive simulation data
* Start Interface to receive/send events from/to interface
* End application
*/

int main() {

    // System config
    sim_interface::SystemConfig systemConfig;
    std::string configPath = std::filesystem::canonical("/proc/self/exe").parent_path().string();
    sim_interface::SystemConfig::loadFromFile(configPath + "/SystemConfig.xml", systemConfig, true);

    // initialize the logger
    sim_interface::InterfaceLogger::initializeLogger(systemConfig.loggerConfig);
    sim_interface::InterfaceLogger::logMessage("Start Application", sim_interface::LOG_LEVEL::INFO);

    // Create interface
    sim_interface::SimToDuTInterface interface;

    // Create simComHandler
    sim_interface::SimComHandler simComHandler(&interface, systemConfig);

    // Init interface with SimComHandler
    interface.setSimComHandler(&simComHandler);
    simComHandler.getConfig();

    // Start simComHandler to receive events from the simulation
    simComHandler.run();

    // Start interface to receive/send events
    interface.run();

    std::cin.get();
    sim_interface::InterfaceLogger::logMessage("Shut down application", sim_interface::LOG_LEVEL::INFO);
    return 0;
}
