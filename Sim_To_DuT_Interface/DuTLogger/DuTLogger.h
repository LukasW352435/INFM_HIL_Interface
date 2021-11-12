/**
 * DuT-Logger
 *
 * Contains the class DuTLogger and all necessary variables.
 * For better documentation see the class "DuTLogger" and it's functions.
 *
 * @author Marco Keul
 * @version 1.0
 */

#ifndef TESTLOGGER_DUTLOGGER_H
#define TESTLOGGER_DUTLOGGER_H
#include <string>
#include "quill/Quill.h"
#include <list>
#include <filesystem>

/**
 * Defines the relative path where the logfiles will be created.
 * In this path the log messages will be logged.
 */
static const std::string PATH_CONSOLE_LOG = "/logs/console";

/**
 * Defines the relative path where the logfiles will be created.
 * In this path the data objects will be logged.
 */
static const std::string PATH_DATA_LOG = "/logs/data";

/**
 * This constant defines the mode how the logger will open the logfile of the console logging.
 * Mode "w" creates a new file or overwrite the existing file at every start of the application.
 * In mode "a" the logger tries to append it's content on an existing file.
 */
static const std::string FILE_MODE_CONSOLE = "a";
/**
 * This constant defines the mode how the logger will open the logfile of the data logging.
 * Mode "w" creates a new file or overwrite the existing file at every start of the application.
 * In mode "a" the logger tries to append it's content on an existing file.
 */
static const std::string FILE_MODE_DATA = "a";

/**
 * This constant defines the maximum number of files in a logging directory. If there are more
 * logfiles under the underlying path, old ones will be deleted. (like a date based file rotation)
 */
static const int FILE_BACKUP_COUNT = 10;

/**
 * Defines the level of logging
 */
enum LOG_LEVEL{
    NONE, DEBUG, INFO, WARNING, ERROR, CRITICAL
};

/**
 * Defines the type and so the task for the logger.
 */
enum LOGGER_TYPE {
    CONSOLE, DATA
};

/**
 * This enum collects all types of logger whose level can be changed
 */
enum LOG_LEVEL_CHANGE_ON {
    CONSOLE_LOG, FILE_LOG
};

/**
 * This constant stores the default level for console logging.
 */
static const quill::LogLevel DEFAULT_CONSOLE_LOG_LEVEL = quill::LogLevel::Info;

/**
 * This constant stores the default level for console file logging.
 */
static const quill::LogLevel DEFAULT_FILE_LOG_LEVEL = quill::LogLevel::Info;

// Little variable to remember that the quill engine has been started.
static bool startedQuillEngine = false;

/**
 * This logger is a tool with specialized functions to log and store messages for all elements of the
 * Sim_To_DuT_Interface. The logger and all of it's functions are static so it's not necessary to create instances
 * of the logger to write a message.
 *
 * In a future version of this logger it will be possible to log event objects. That funcion haven't been implemented
 * so far because the definition of these events is not finished yet.
 *
 * The logger will create files where necessary message will be stored. The message provider can choose if the
 * message should be logged in the file or not. The logger notices old logfiles and will remove them if the real
 * number of logfiles is higher than the configured backup count.
 *
 * @author Marco Keul
 * @version 1.0
 */
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
    static std::string currentLogpathConsole;
    static std::string currentLogpathData;

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
    static void logWithLevel(quill::Logger* log, std::string msg, LOG_LEVEL level);
    static quill::Handler* getHandlerType(LOG_LEVEL_CHANGE_ON type);

    // file management
    static std::string getLoggingPath(LOGGER_TYPE type);
    static std::string initializeLoggingPath(LOGGER_TYPE type);
    static void removeOldLogfiles(std::string directory);
};


#endif //TESTLOGGER_DUTLOGGER_H
