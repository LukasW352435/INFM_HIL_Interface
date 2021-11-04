//
// Created by marco on 26.10.21.
//

#ifndef TESTLOGGER_DUTLOGGER_H
#define TESTLOGGER_DUTLOGGER_H
#include <string>
#include "quill/Quill.h"
#include <filesystem>

static const std::string PATH_CONSOLE_LOG = "/logs/console";
static const std::string PATH_DATA_LOG = "/logs/data";

enum LOG_LEVEL{
    NONE, DEBUG, INFO, WARNING, ERROR, CRITICAL
};

enum LOGGER_TYPE {
    CONSOLE, DATA
};

enum LOG_LEVEL_CHANGE_ON {
    CONSOLE_LOG, FILE_LOG
};

// pre-settings
// feel free to change the default level for logging
static const quill::LogLevel DEFAULT_CONSOLE_LOG_LEVEL = quill::LogLevel::Info;
static const quill::LogLevel DEFAULT_FILE_LOG_LEVEL = quill::LogLevel::Info;

static bool startedQuillEngine = false;


class DuTLogger {

public:
    static void logMessage(std::string msg, LOG_LEVEL level);
    static void logMessage(std::string msg, LOG_LEVEL level, bool writeToFile);
    static void changeLogLevel(LOG_LEVEL_CHANGE_ON typ, LOG_LEVEL level);

private:
    // define all required loggers
    static quill::Logger* consoleLogger;
    static quill::Logger* consoleFileLogger;
    static quill::Logger* dataLogger;

    // handlers (important to change the log_level
    static quill::Handler* consoleHandler;
    static quill::Handler* consoleFileHandler;
    static quill::Handler* buildConsoleHandler();
    static quill::Handler* buildFileHandler();

    // functions to create and manage loggers
    static quill::Logger* createConsoleLogger(const char* name, bool withFileHandler);
    static quill::Logger* createDataLogger();
    static void startEngine();

    // help functions
    static std::string getLoggingPath(LOGGER_TYPE type);
    static void createDirectoryIfNecessary(const std::string path);
    static void logWithLevel(quill::Logger* log, std::string msg, LOG_LEVEL level);
    static quill::Handler* getHandlerType(LOG_LEVEL_CHANGE_ON type);
};


#endif //TESTLOGGER_DUTLOGGER_H
