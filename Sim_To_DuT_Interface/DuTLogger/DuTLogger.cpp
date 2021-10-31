//
// Created by marco on 26.10.21.
//

#include "DuTLogger.h"

// initialize the static handlers
quill::Handler* DuTLogger::consoleHandler = DuTLogger::buildConsoleHandler();
quill::Handler* DuTLogger::consoleFileHandler = DuTLogger::buildFileHandler();

// initialize the static loggers
quill::Logger* DuTLogger::consoleLogger = DuTLogger::createConsoleLogger("consoleLog", false);
quill::Logger* DuTLogger::consoleFileLogger = DuTLogger::createConsoleLogger("consoleFileLog", true);
quill::Logger* DuTLogger::dataLogger = DuTLogger::createDataLogger();

quill::Logger* DuTLogger::createConsoleLogger(const char* name, bool withFileHandler) {
    // check if the quill engine is started
    startEngine();

    // check if the new logger needs to write the console log to a file
    // if it needs it, we have to build the logger a different way
    quill::Logger* newLogger;
    if (withFileHandler) {
        newLogger = quill::create_logger(name, {consoleHandler, consoleFileHandler});
    } else {
        newLogger = quill::create_logger(name, consoleHandler);
    }

    // Set the LogLevel (L3 for everything)
    newLogger->set_log_level(quill::LogLevel::TraceL3);

    return newLogger;
}

void DuTLogger::startEngine() {
    // start the quill engine if it hasn't been started yet
    if (!startedQuillEngine) {
        quill::enable_console_colours();
        quill::start();

        // remember that we started the engine
        startedQuillEngine = true;
    }
}

quill::Handler* DuTLogger::buildConsoleHandler() {
    // build a handler for the console
    quill::Handler* newHandler = consoleHandler = quill::stdout_handler("consoleHandler");
    newHandler->set_log_level(DEFAULT_CONSOLE_LOG_LEVEL);

    // modify the pattern for the logger
    newHandler->set_pattern(QUILL_STRING("%(ascii_time)  %(level_name): %(message)"),
                                "%D %H:%M:%S.%Qms");
    return newHandler;
}

quill::Handler* DuTLogger::buildFileHandler() {
    // get the path to the log file or create the directory if necessary
    std::string path = DuTLogger::getLoggingPath(LOGGER_TYP::CONSOLE);
    createDirectoryIfNecessary(path);

    // a second handler for the file is needed
    quill::Handler* newHandler = quill::file_handler(path.append("/Logfile.log"), "w");
    newHandler->set_log_level(DEFAULT_FILE_LOG_LEVEL);

    // modify the pattern for the logger
    newHandler->set_pattern(QUILL_STRING("%(ascii_time)  %(level_name): %(message)"),
                            "%D %H:%M:%S.%Qms");

    return newHandler;
}

quill::Logger* DuTLogger::createDataLogger() {
    // get the path to the log file or create the directory if necessary
    std::string path = DuTLogger::getLoggingPath(LOGGER_TYP::DATA);
    createDirectoryIfNecessary(path);

    // create a file handler to connect quill to a logfile
    quill::Handler* file_handler = quill::file_handler(path.append("/Logfile.log"), "w");

    // configure the pattern of a line
    file_handler->set_pattern(QUILL_STRING("%(ascii_time) %(logger_name) - %(message)"),
            "%D %H:%M:%S.%Qms %z", quill::Timezone::LocalTime);

    // finally, create the logger and return it
    quill::Logger* createdLogger = quill::create_logger("dataLog", file_handler);

    return createdLogger;
}

std::string DuTLogger::getLoggingPath(LOGGER_TYP typ) {
    // get the path, where this program is running
    std::string path = std::filesystem::current_path();

    // modify the path, so it leads to our logfile
    std::string result = path.substr(0, path.find_last_of("/"));

    // get the right path for the logger
    if (typ == LOGGER_TYP::DATA) {
        result.append(PATH_DATA_LOG);
    } else {
        result.append(PATH_CONSOLE_LOG);
    }
    // return the path
    return result;
}

void DuTLogger::createDirectoryIfNecessary(const std::string path) {
    // check if the directory is created in the file system
    std::filesystem::create_directories(path);
}

void DuTLogger::disableConsoleToFileLogging(bool disable) {
    enableConsoleToFile = !disable;
}

void DuTLogger::changeLogLevel(LOG_LEVEL_CHANGE_ON typ, LOG_LEVEL level) {
    quill::Handler* handler = DuTLogger::getHandlerTyp(typ);

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
            logMessage("Can not change LogLevel! Invalid argument!", LOG_LEVEL::ERROR);
            handler->set_log_level(quill::LogLevel::TraceL3);
            break;
    }
}

quill::Handler* DuTLogger::getHandlerTyp(LOG_LEVEL_CHANGE_ON typ) {
    quill::Handler* handler;

    switch (typ) {
        case LOG_LEVEL_CHANGE_ON::CONSOLE_LOG:
            handler = consoleHandler;
            break;

        case LOG_LEVEL_CHANGE_ON::FILE_LOG:
            handler = consoleFileHandler;
            break;

        default:
            throw std::invalid_argument("Internal Error! Unknown Typ of <LOG_LEVEL_CHANGED_ON> appeared.");
    }

    return handler;
}

void DuTLogger::logMessage(std::string msg, LOG_LEVEL level) {
    // check for the right logger
    quill::Logger* log;
    if (enableConsoleToFile) {
        log = consoleFileLogger;
    } else {
        log = consoleLogger;
    }

    // log the msg with the specific logger
    logWithLevel(log, msg, level);
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
            throw std::invalid_argument("Parsed unknown LOG_LEVEL <" + msg + ">");
    }
}