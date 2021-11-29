/**
 * Implementation of DuTLogger.h
 *
 * @author Marco Keul
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


void DuTLogger::initializeLogger(const LoggerConfig con) {
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

/**
 * Creates a handler to write messages in the console.
 * This handler can be connected to a logger.
 *
 * @return a handler to log messages in the console
 */
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

/**
 * Creates a handler to write messages into a file.
 * This handler can be connected to a logger.
 * Depending on the configuration the handler will create a new file for every start or try to append on the current one.
 *
 * @return a handler to log messages in a file
 */
quill::Handler* DuTLogger::buildFileHandler(std::string logPath, bool enableDebugMode) {
    // a second handler for the file is needed
    std::string basicPath = logPath + "/Logfile_" + getCurrentTimestamp() + ".log";
    quill::Handler* newHandler = quill::file_handler(basicPath, FILE_MODE_CONSOLE,
                                                     quill::FilenameAppend::None);

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

/**
 * Builds a logger for the console. If necessary the logger will be build with an additional handler for
 * files. This will be configured by the second argument.
 *
 * @param name the name for the logger
 * @param withFileHandler true if the logger also needs a handler for logfiles
 * @return the created logger
 */
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

/**
 * Creates a logger to log a specific data object (like events; not normal messages!) into a file.
 * The logger will be build with all necessary handlers.
 * Depending on the configuration the handler will create a new file for every start or try to append on the current one.
 *
 * @return a logger to log data objects into a file
 */
quill::Logger* DuTLogger::createDataLogger(std::string logPath) {
    // create a file handler to connect quill to a logfile
    std::string basicPath = logPath + "/Logfile_" + getCurrentTimestamp() + ".csv";
    quill::Handler* file_handler = quill::file_handler(basicPath, FILE_MODE_DATA,quill::FilenameAppend::None);

    // configure the pattern of a line
    file_handler->set_pattern(QUILL_STRING("%(message)"));

    // finally, create the logger and return it
    quill::Logger* createdLogger = quill::create_logger("dataLog", file_handler);

    return createdLogger;
}

/**
 * This function returns a valid path to the logfiles. To handle this task it will read the configurated path.
 * If necessary it will create the path.
 * @param type the type of logger
 * @return a valid path for creating and modifying logfiles
 */
std::string DuTLogger::initializeLoggingPath(std::string logPath) {
    // get the wished logging path for this type of logger
    std::string path = getLoggingPath(logPath);

    // create the log directory if it doesn't exist
    std::filesystem::create_directories(path);
    return path;
}

/**
 * Identifies the path to the log file for the specific typ of logger by using the underlying path configuration.
 * The function can handle absolute and relative paths from the configuration.
 *
 * @param type  - type of logger
 * @return path to the logfile
 */
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
    std::string result = path.substr(0, path.find_last_of("/"));

    // append the specified logging path for the logger on the path
    result.append(logPath);

    // return the path
    return result;
}

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
void DuTLogger::removeOldLogfiles(std::string directory, int backupCount) {
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
        std::list<std::string>::iterator iter = allLogFiles.begin();
        for (int i = 0; i < (allLogFiles.size() - backupCount); i++) {
            // get the path to the file and remove it
            std::string file = *iter;
            std::remove(file.c_str());          // cast string to const char*

            // set the iterator on the next object for the next iteration
            std::advance(iter, 1);
        }
    }
}

/**
 * Change the logging level for a specific logger. Because there are multiple loggers you have to
 * select for which type of logger you want to change the level.
 * @param type the type of logger
 * @param level new level for logging
 */
void DuTLogger::changeLogLevel(LOG_TYPE type, LOG_LEVEL level) {
    if (!initialized) {
        std::cerr << "Logger has not been initialized. Please parse a config to the logger." << std::endl;
    }

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

/**
 * Returns the right quill handler for the given type of logger
 * @param type type of logger
 * @return connected handler
 */
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

/**
 * Logs the message with consideration of the log level. Please notice that this function will also log the message
 * into a logfile if the file logger accepts the log level. If you explicitly don't want to log this message into the
 * logfile, please use the function below.
 *
 * @see void DuTLogger::logMessage(std::string msg, LOG_LEVEL level, bool doNotWriteIntoFile)
 * @param msg message that should be logged
 * @param level the logging level for this message
 */
void DuTLogger::logMessage(std::string msg, LOG_LEVEL level) {
    if (!initialized) {
        std::cerr << "Logger has not been initialized. Please parse a config to the logger." << std::endl;
    }

    // hand the message with the right level to the quill framework.
    // if the user didn't choose to log the message into the logfile -> try to log it with the level
    // to the file, the level on the fileLogger will decide if the msg will be written
    logWithLevel(consoleFileLogger, msg, level);
}

/**
 * Logs the message with consideration of the log level. This function gives you additionally the possibility
 * to prevent that the logger will write your message into the logfile. So if you set the 3rd parameter to 'false'
 * the logger won't log your message into the file even if the log level would have accepted it.
 *
 * @param msg message that should be logged
 * @param level the logging level for this message
 * @param doNotWriteIntoFile true, if you don't want to log into the logfile
 */
void DuTLogger::logMessage(std::string msg, LOG_LEVEL level, bool doNotWriteIntoFile) {
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

/**
 * This help function will parse the message to the quill engine.
 *
 * @param log the specific logger that should be used
 * @param msg the message that should be logged
 * @param level the logging level
 */
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

/**
 * This function returns a string with the current time. This string can be used to name files.
 *
 * @return the current time as string
 */
std::string DuTLogger::getCurrentTimestamp() {
    // look up the local time
    auto t = std::time(nullptr);
    auto timer = *std::localtime(&t);

    // write it formatted in a stream and convert that to a string so we can return it
    std::ostringstream oss;
    oss << std::put_time(&timer, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

/**
 * This function logs the event to the data logfiles. There is no need to define a logging level for this operation.
 * <br>
 * Please notice, that characters like comma or double quotes causes trouble in CSV files and will be replaced by using
 * standard rules.
 *
 * @param event This event will be logged.
 */
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
