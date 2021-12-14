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

#include "DuTLogger.h"

// set the initialized variable to false at start
bool DuTLogger::initialized = false;

// initialize the static handlers
quill::Handler* DuTLogger::consoleHandler;
quill::Handler* DuTLogger::consoleFileHandler;

// initialize the static loggers
quill::Logger* DuTLogger::consoleLogger;
quill::Logger* DuTLogger::consoleFileLogger;
quill::Logger* DuTLogger::dataLogger;

/**
 * Using std::chrono clock instead of the default rdtsc clock
 */
#define QUILL_CHRONO_CLOCK

void DuTLogger::initializeLogger(const LoggerConfig &con) {
    if (initialized) {
        logMessage("Logger can't be initialized again!", LOG_LEVEL::ERROR);
        return;
    }
    // start the quill engine
    quill::start();

    // create the logging paths
    std::string logPathConsole = initializeLoggingPath(con.pathConsoleLog);
    std::string logPathData = initializeLoggingPath(con.pathDataLog);

    DuTLogger::consoleHandler = DuTLogger::buildConsoleHandler(con.enableDebugMode);
    DuTLogger::consoleFileHandler = DuTLogger::buildFileHandler(logPathConsole, con.enableDebugMode);

    DuTLogger::consoleLogger = DuTLogger::createConsoleLogger("consoleLog", false);
    DuTLogger::consoleFileLogger = DuTLogger::createConsoleLogger("consoleFileLog", true);
    DuTLogger::dataLogger = DuTLogger::createDataLogger(logPathData);

    // check if we might have to delete old logging folders
    removeOldLogfiles(logPathConsole, con.fileBackupCount);
    removeOldLogfiles(logPathData, con.fileBackupCount);

    // initialize the CSV file for data logging
    LOG_INFO(dataLogger, "{}", CSV_HEADER);

    // remember that we've initialized the logger
    initialized = true;

    // check if the user want to use another logging level than the default one
    if (con.fileLogLevel != LOG_LEVEL::INFO) {
        changeLogLevel(LOG_TYPE::FILE_LOG, con.fileLogLevel);
    }
    if (con.consoleLogLevel != LOG_LEVEL::INFO) {
        changeLogLevel(LOG_TYPE::CONSOLE_LOG, con.consoleLogLevel);
    }
}

quill::Handler* DuTLogger::buildConsoleHandler(bool enableDebugMode) {
    // build a handler for the console
    quill::Handler* newHandler = quill::stdout_handler("consoleHandler");

    // Check if debug mode is enabled. If not use the configured default level
    if (enableDebugMode) {
        newHandler->set_log_level(quill::LogLevel::Debug);
    } else {
        newHandler->set_log_level(quill::LogLevel::Info);
    }

    // modify the pattern for the logger
    newHandler->set_pattern(QUILL_STRING("%(ascii_time)  %(level_name): %(message)"),
                                "%D %H:%M:%S.%Qms");
    return newHandler;
}

quill::Handler* DuTLogger::buildFileHandler(const std::string &logPath, bool enableDebugMode) {
    // a second handler for the file is needed
    std::string basicPath = logPath + "/Logfile_" + getCurrentTimestamp() + ".log";
    quill::Handler* newHandler = quill::file_handler(basicPath, "w", quill::FilenameAppend::None);

    // Check if debug mode is enabled. If not use the configured default level
    if (enableDebugMode) {
        newHandler->set_log_level(quill::LogLevel::Debug);
    } else {
        newHandler->set_log_level(quill::LogLevel::Info);
    }

    // modify the pattern for the logger.
    newHandler->set_pattern(QUILL_STRING("%(ascii_time)  %(level_name): %(message)"),
                            "%D %H:%M:%S.%Qms");

    // return the new handler
    return newHandler;
}

quill::Logger* DuTLogger::createConsoleLogger(const char* name, bool withFileHandler) {
    // check if the new logger needs to write the console log to a file
    // if needed, we have to build the logger in a different way
    quill::Logger* newLogger;
    if (withFileHandler) {
        newLogger = quill::create_logger(name, {consoleHandler, consoleFileHandler});
    } else {
        newLogger = quill::create_logger(name, consoleHandler);
    }

    // Define the deepest possible log_level for this logger
    // the handlers can log higher than him. So if we want to change the level, we change the level of the handlers
    // a handler can't log deeper than the logger
    newLogger->set_log_level(quill::LogLevel::Debug);

    return newLogger;
}

quill::Logger* DuTLogger::createDataLogger(const std::string &logPath) {
    // create a file handler to connect quill to a logfile
    std::string basicPath = logPath + "/Logfile_" + getCurrentTimestamp() + ".csv";
    quill::Handler* file_handler = quill::file_handler(basicPath, "w",quill::FilenameAppend::None);

    // configure the pattern of a line
    file_handler->set_pattern(QUILL_STRING("%(message)"));

    // finally, create the logger and return it
    quill::Logger* createdLogger = quill::create_logger("dataLog", file_handler);

    return createdLogger;
}

std::string DuTLogger::initializeLoggingPath(const std::string &logPath) {
    // get the wished logging path for this type of logger
    std::string path = getLoggingPath(logPath);

    // create the log directory if it doesn't exist
    std::filesystem::create_directories(path);
    return path;
}

std::string DuTLogger::getLoggingPath(std::string logPath) {
    // Check if the user provided an absolute path for logging
    // If this is true -> remove the first identifier
    if (logPath.at(0) == '#') {
        return logPath.substr(1, logPath.size()-1);
    }

    // the user provided a relative path -> build the relative path
    // get the path, where this program is running
    std::string path = std::filesystem::current_path();

    // modify the path, so it leads to our logfile
    std::string result = path.substr(0, path.find_last_of('/'));

    // append the specified logging path for the logger on the path
    result.append(logPath);

    // return the path
    return result;
}

void DuTLogger::removeOldLogfiles(const std::string &directory, int backupCount) {
    // collect all files under this directory in a list
    std::list<std::string> allLogFiles;
    for (const auto & entry : std::filesystem::directory_iterator(directory)) {
        allLogFiles.push_back(entry.path());
    }

    // check if we have more files than allowed by the backup count
    if (allLogFiles.size() > backupCount) {
        // sort the list alphabetically so the oldest file will be the first one
        allLogFiles.sort();

        // get the iterator for the list and search for old files to remove them
        // remove so much files until we have the accepted number again
        // remove from the front, because there will be the old files
        auto iter = allLogFiles.begin();
        for (int i = 0; i < (allLogFiles.size() - backupCount); i++) {
            // get the path to the file and remove it
            std::string file = *iter;
            std::remove(file.c_str());          // cast string to const char*

            // set the iterator on the next object for the next iteration
            std::advance(iter, 1);
        }
    }
}

void DuTLogger::changeLogLevel(LOG_TYPE type, LOG_LEVEL level) {
    if (!initialized) {
        std::cerr << "Logger has not been initialized. Please parse a config to the logger." << std::endl;
    }
    // get the right handler. We want to make changes on this one.
    quill::Handler* handler = DuTLogger::getHandlerType(type);

    // log all messages before changing the level
    quill::flush();

    switch (level) {
        case LOG_LEVEL::NONE:
            handler->set_log_level(quill::LogLevel::None);
            break;

        case LOG_LEVEL::DEBUG:
            handler->set_log_level(quill::LogLevel::Debug);
            break;

        case LOG_LEVEL::INFO:
            handler->set_log_level(quill::LogLevel::Info);
            break;

        case LOG_LEVEL::WARNING:
            handler->set_log_level(quill::LogLevel::Warning);
            break;

        case LOG_LEVEL::ERROR:
            handler->set_log_level(quill::LogLevel::Error);
            break;

        case LOG_LEVEL::CRITICAL:
            handler->set_log_level(quill::LogLevel::Critical);
            break;

        default:
            logMessage("Can not change LogLevel! Invalid argument!", LOG_LEVEL::ERROR, true);
            handler->set_log_level(quill::LogLevel::TraceL3);
            break;
    }
}

quill::Handler* DuTLogger::getHandlerType(LOG_TYPE type) {
    quill::Handler* handler;

    switch (type) {
        case LOG_TYPE::CONSOLE_LOG:
            handler = consoleHandler;
            break;

        case LOG_TYPE::FILE_LOG:
            handler = consoleFileHandler;
            break;

        default:
            throw std::invalid_argument("Internal Error! Unknown Typ of <LOG_TYPE> appeared.");
    }

    return handler;
}

void DuTLogger::logMessage(const std::string &msg, LOG_LEVEL level) {
    if (!initialized) {
        std::cerr << "Logger has not been initialized. Please parse a config to the logger." << std::endl;
    }

    // hand the message with the right level to the quill framework.
    // if the user didn't choose to log the message into the logfile -> try to log it with the level
    // to the file, the level on the fileLogger will decide if the msg will be written
    logWithLevel(consoleFileLogger, msg, level);
}

void DuTLogger::logMessage(const std::string &msg, LOG_LEVEL level, bool doNotWriteIntoFile) {
    if (!initialized) {
        std::cerr << "Logger has not been initialized. Please parse a config to the logger." << std::endl;
    }

    // check if we have to write the message to the LogFile
    if (doNotWriteIntoFile) {
        logWithLevel(consoleLogger, msg, level);
    } else {
        logWithLevel(consoleFileLogger, msg, level);
    }
}

void DuTLogger::logWithLevel(quill::Logger* log, std::string msg, LOG_LEVEL level) {
    // parse the message to the right will logger with the given level
    switch (level) {
        case LOG_LEVEL::NONE:
            LOG_WARNING(log, "Can't log this message, because the chosen Log_Level is <NONE>");
            break;

        case LOG_LEVEL::DEBUG:
            LOG_DEBUG(log, "{}", msg);
            break;

        case LOG_LEVEL::INFO:
            LOG_INFO(log, "{}", msg);
            break;

        case LOG_LEVEL::WARNING:
            LOG_WARNING(log, "{}", msg);
            break;

        case LOG_LEVEL::ERROR:
            LOG_ERROR(log, "{}", msg);
            break;

        case LOG_LEVEL::CRITICAL:
            LOG_CRITICAL(log, "{}", msg);
            break;

        default:
            throw std::invalid_argument("Parsed unknown LOG_LEVEL!");
    }
}

std::string DuTLogger::getCurrentTimestamp() {
    // look up the local time
    auto t = std::time(nullptr);
    auto timer = *std::localtime(&t);

    // write it formatted in a stream and convert that to a string, so we can return it
    std::ostringstream oss;
    oss << std::put_time(&timer, "%Y-%m-%d_%H:%M:%S");
    return oss.str();
}

void DuTLogger::logEvent(sim_interface::SimEvent event) {
    if (!initialized) {
        std::cerr << "Logger has not been initialized. Please parse a config to the logger." << std::endl;
    }

    // because the value of the event can have different types we have to use format this type into a string to log it.
    // To solve this problem a boost::static_visitor class has been created. This class can cast values of
    // defined types into strings
    std::string convertedValue = boost::apply_visitor(EventVisitor(), event.value);

    // log the event with the converted value
    // don't forget to replace character, that will cause problems in CSV files
    LOG_INFO(dataLogger, "{},{},{},{}", EventVisitor::checkForSpecialChars(event.operation),
                                        convertedValue,
                                        EventVisitor::checkForSpecialChars(event.origin),
                                        event.current);
}
