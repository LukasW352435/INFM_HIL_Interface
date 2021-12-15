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
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_INTERFACELOGGER_H
#define SIM_TO_DUT_INTERFACE_INTERFACELOGGER_H

#include "InterfaceLoggerConfig.h"
#include "../Events/EventVisitor.h"
#include <string>
#include "quill/Quill.h"
#include "../Events/SimEvent.h"
#include <list>
#include <filesystem>
#include <fstream>

/**
 * This constant contains the header of the csv files. This header will be printed in the first line of the files and
 * represents the order and meaning of the logged values.
 */
static const std::string CSV_HEADER = "Operation,Value,Origin,Timestamp";

/**
 * This enum collects all types of logger whose level can be changed.
 * So there is a log level for logging messages in the console and another one to log messages in a file.
 * Please notice that you can't change the level of the data logging, because all data objects will be logged with
 * the same level.
 */
enum LOG_TYPE {
    CONSOLE_LOG, FILE_LOG
};

/**
 * This logger is a tool with specialized functions to log and store messages for all elements of the
 * Sim_To_DuT_Interface. The logger and all of it's functions are static so it's not necessary to create instances
 * of the logger to write a message. Please notice that the logger has to be initialised in a first call by a valid
 * configuration before using it.
 *
 * With logger's functionality it's possible to log messages and events during the execution.
 *
 * The logger will create files if necessary in which the messages will be stored. The message provider can choose if the
 * message should be logged in the file or not. The logger notices old logfiles and will remove them if the real
 * number of logfiles is higher than the configured backup count.
 *
 * @author Marco Keul
 * @version 1.0
 */
class InterfaceLogger {

public:

    /**
     * This function initializes the logger with the given configuration. After calling this function it's possible
     * to log messages and data objects.
     *
     * @param con   - a valid configuration
     */
    static void initializeLogger(const LoggerConfig &con);

    /**
     * Logs the message with consideration of the log level. Please notice that this function will also log the message
     * into a logfile if the file logger accepts the log level. If you explicitly don't want to log this message into the
     * logfile, please use the function below.
     *
     * @see void InterfaceLogger::logMessage(std::string msg, LOG_LEVEL level, bool doNotWriteIntoFile)
     * @param msg message that should be logged
     * @param level the logging level for this message
     */
    static void logMessage(const std::string &msg, LOG_LEVEL level);

    /**
     * Logs the message with consideration of the log level. This function gives you additionally the possibility
     * to prevent that the logger will write your message into the logfile. So if you set the 3rd parameter to 'false'
     * the logger won't log your message into the file even if the log level would have accepted it.
     *
     * @param msg message that should be logged
     * @param level the logging level for this message
     * @param doNotWriteIntoFile true, if you don't want to log into the logfile
     */
    static void logMessage(const std::string &msg, LOG_LEVEL level, bool writeToFile);

    /**
     * Change the logging level for a specific logger. Because there are multiple loggers you have to
     * select for which type of logger you want to change the level.
     * @param type the type of logger
     * @param level new level for logging
     */
    static void changeLogLevel(LOG_TYPE typ, LOG_LEVEL level);

    /**
     * This function logs the event to the data logfiles. There is no need to define a logging level for this operation.
     * <br>
     * Please notice, that characters like comma or double quotes causes trouble in CSV files and
     * will be replaced by using standard rules.
     *
     * @param event This event will be logged.
     */
    static void logEvent(sim_interface::SimEvent event);

private:
    // variable to remember if the logger has been initialized yet
    static bool initialized;

    // define all required loggers
    static quill::Logger* consoleLogger;
    static quill::Logger* consoleFileLogger;
    static quill::Logger* dataLogger;

    // handlers (important to change the log_level
    static quill::Handler* consoleHandler;
    static quill::Handler* consoleFileHandler;

    /**
     * Creates a handler to write messages in the console.
     * This handler can be connected to a logger.
     *
     * @return a handler to log messages in the console
     */
    static quill::Handler* buildConsoleHandler(bool enableDebugMode);

    /**
     * Creates a handler to write messages into a file.
     * This handler can be connected to a logger.
     * Depending on the configuration the handler will create a new file for every start or try
     * to append on the current one.
     *
     * @return a handler to log messages in a file
     */
    static quill::Handler* buildFileHandler(const std::string &logPath, bool enableDebugMode);

    /**
     * Builds a logger for the console. If necessary the logger will be build with an additional handler for
     * files. This will be configured by the second argument.
     *
     * @param name the name for the logger
     * @param withFileHandler true if the logger also needs a handler for logfiles
     * @return the created logger
     */
    static quill::Logger* createConsoleLogger(const char* name, bool withFileHandler);

    /**
     * Creates a logger to log a specific data object (like events; not normal messages!) into a file.
     * The logger will be build with all necessary handlers.
     * Depending on the configuration the handler will create a new file for every start or try to append on the current one.
     *
     * @return a logger to log data objects into a file
     */
    static quill::Logger* createDataLogger(const std::string &logPath);

    /**
     * This help function will parse the message to the quill engine.
     *
     * @param log the specific logger that should be used
     * @param msg the message that should be logged
     * @param level the logging level
     */
    static void logWithLevel(quill::Logger* log, std::string msg, LOG_LEVEL level);

    /**
     * Returns the right quill handler for the given type of logger
     * @param type type of logger
     * @return connected handler
     */
    static quill::Handler* getHandlerType(LOG_TYPE type);

    /**
     * This function returns a string with the current time. This string can be used to name files.
     *
     * @return the current time as string
     */
    static std::string getCurrentTimestamp();

    /**
     * Identifies the path to the log file for the specific typ of logger by using the underlying path configuration.
     * The function can handle absolute and relative paths from the configuration.
     *
     * @param type  - type of logger
     * @return path to the logfile
     */
    static std::string getLoggingPath(std::string logPath);

    /**
     * This function returns a valid path to the logfiles. To handle this task it will read the configured path.
     * If necessary it will create the path.
     * @param type the type of logger
     * @return a valid path for creating and modifying logfiles
     */
    static std::string initializeLoggingPath(const std::string &logPath);

    /**
     * This function should be called on every startup of the application.
     * It deletes old logfiles in the given logging directory.
     *
     * Which files will be deleted depends on the configured backup count. If the current number of files is higher
     * than the allowed number by the backup count, old files will be deleted until we have the allowed number again.
     *
     * Please notice that on every start of the application the file handlers will automatically create a new file,
     * if there isn't already a file for the day.
     *
     * @param directory old logfiles will be deleted in this directory
     */
    static void removeOldLogfiles(const std::string &directory, int backupCount);
};

#endif //SIM_TO_DUT_INTERFACE_INTERFACELOGGER_H
