/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Marco Keul
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
 * @author Marco Keul
 * @author Lukas Wagenlehner
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_INTERFACELOGGERCONFIG_H
#define SIM_TO_DUT_INTERFACE_INTERFACELOGGERCONFIG_H

#include <string>
#include <cassert>
#include <stdexcept>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace sim_interface {

    /**
     * Defines the level of logging
     */
    enum LOG_LEVEL {
        NONE, DEBUG, INFO, WARNING, ERROR, CRITICAL
    };

    /**
     * <summary>
     * The logger needs a configuration before it can start logging.
     * </summary>
     * This configuration has to store all required information that the logger needs. Default settings are provided.
     */
    class LoggerConfig {
    public:

        /**
         * Creates the essential configuration for the logger with default settings.
         */
        LoggerConfig() = default;

        /**
         * Create the essential configuration for the logger.
         * <br>
         * For the logging paths it's possible to enter relative and absolute file paths. Both variants will be accepted.
         * An example for a relative path is "/logs/console". Please notice that there is no '/' at the end.
         * If you rather want to define an absolute path please add an additional '#' in front of your absolute path.
         * An example for an absolute path is "#/home/user/project/logs/console"
         *
         * @param enableDebugMode   - enables debug mode for logging. Logging levels will be ignored.
         * @param pathConsoleLog    - contains the path to write the console log into a file
         * @param pathDataLog       - contains the path to write the data log into a file
         * @param fileBackupCount   - defines maximum amount of files in a logging directory. Old files will be deleted.
         * @param fileLogLevel      - defines the logging level for the file log
         * @param consoleLogLevel   - defines the logging level for the console log
         */
        LoggerConfig(bool enableDebugMode, std::string pathConsoleLog, std::string pathDataLog, int fileBackupCount,
                     LOG_LEVEL fileLogLevel, LOG_LEVEL consoleLogLevel)
                : enableDebugMode(std::move(enableDebugMode)), pathConsoleLog(std::move(pathConsoleLog)),
                  pathDataLog(std::move(pathDataLog)), fileBackupCount(std::move(fileBackupCount)),
                  fileLogLevel(std::move(fileLogLevel)), consoleLogLevel(std::move(consoleLogLevel)) {
            if (this->fileBackupCount <= 0) {
                throw std::invalid_argument("FileBackupCount must be greater 0");
            }
            if (this->pathConsoleLog.empty()) {
                throw std::invalid_argument("The path for consoleLog is not allowed to be empty! May use default configuration.");
            }
            if (this->pathConsoleLog.empty()) {
                throw std::invalid_argument("The path for consoleLog is not allowed to be empty! May use default configuration.");
            }
        }

        bool enableDebugMode = false;
        std::string pathConsoleLog = "/logs/console";
        std::string pathDataLog = "/logs/data";
        int fileBackupCount = 10;
        LOG_LEVEL fileLogLevel = LOG_LEVEL::INFO;
        LOG_LEVEL consoleLogLevel = LOG_LEVEL::INFO;
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & BOOST_SERIALIZATION_NVP(enableDebugMode);
            ar & BOOST_SERIALIZATION_NVP(pathConsoleLog);
            ar & BOOST_SERIALIZATION_NVP(pathDataLog);
            ar & BOOST_SERIALIZATION_NVP(fileBackupCount);
            ar & BOOST_SERIALIZATION_NVP(fileLogLevel);
            ar & BOOST_SERIALIZATION_NVP(consoleLogLevel);
        }
    };

}

#endif //SIM_TO_DUT_INTERFACE_INTERFACELOGGERCONFIG_H
