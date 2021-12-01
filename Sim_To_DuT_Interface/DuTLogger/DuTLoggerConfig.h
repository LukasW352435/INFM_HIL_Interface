/**
 * The DuTLoggerConfig contains essential values to configure the DuTLogger.
 * Every value will be explained in the documentation of this file.
 *
 * @author Marco Keul
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_DUTLOGGERCONFIG_H
#define SIM_TO_DUT_INTERFACE_DUTLOGGERCONFIG_H

#include <string>
#include <cassert>

/**
 * Defines the level of logging
 */
enum LOG_LEVEL{
    NONE, DEBUG, INFO, WARNING, ERROR, CRITICAL
};

/**
 * The logger needs a configuration before it can start logging. This configuration has to store all required
 * information that the logger needs. Default settings are provided.
 */
class LoggerConfig {
public:

    /**
     * Creates the essential configuration for the logger with default settings.
     */
    LoggerConfig() {}

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
        assert (this->fileBackupCount > 0);
        assert (!this->pathConsoleLog.empty());
        assert (!this->pathDataLog.empty());
    }

    bool enableDebugMode = false;
    std::string pathConsoleLog = "/logs/console";
    std::string pathDataLog = "/logs/data";
    int fileBackupCount = 10;
    LOG_LEVEL fileLogLevel = LOG_LEVEL::INFO;
    LOG_LEVEL consoleLogLevel = LOG_LEVEL::INFO;
};


#endif //SIM_TO_DUT_INTERFACE_DUTLOGGERCONFIG_H
