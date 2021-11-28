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
#include "quill/Quill.h"

/**
 * If this constant is true, the debug mode for logging is activated. In debug mode the console will show all
 * debug messages and write all of them in the logfile. The default logging level will be ignored.
 */
static const bool ENABLE_DEBUG_MODE = false;

/**
 * Defines the path where the console messages will be logged.
 * It is possible to use a relative path. In this case the configured below will be appended on the path of
 * application's execution. An example for a relative path is "/logs/console".
 * Please notice that there is no '/' at the end.
 * If you rather want to define an absolute path please add an additional '#' in front of your absolute path.
 * An example for an absolute path is "#/home/user/project/logs/console"
 */
static const std::string PATH_CONSOLE_LOG = "/logs/console";

/**
 * Defines the path where the data objects will be logged.
 * It is possible to use a relative path. In this case the configured below will be appended on the path of
 * application's execution. An example for a relative path is "/logs/data".
 * Please notice that there is no '/' at the end.
 * If you rather want to define an absolute path please add an additional '#' in front of your absolute path.
 * An example for an absolute path is "#/home/user/project/logs/data"
 */
static const std::string PATH_DATA_LOG = "/logs/data";

/**
 * This constant defines the maximum number of files in a logging directory. If there are more
 * logfiles under the underlying path, old ones will be deleted. (like a date based file rotation)
 */
static const int FILE_BACKUP_COUNT = 10;

/**
 * This constant stores the default level for console logging.
 * Please define here the start logging level you would like to log with.
 * If the debug mode is on, this level will be ignored!
 *
 * During the execution the level can be manually changed by the function below.
 *
 * @see void changeLogLevel(LOG_LEVEL_CHANGE_ON typ, LOG_LEVEL level)
 */
static const quill::LogLevel DEFAULT_CONSOLE_LOG_LEVEL = quill::LogLevel::Info;

/**
 * This constant stores the default level for console file logging.
 * Please define here the start logging level you would like to log with.
 * If the debug mode is on, this level will be ignored!
 *
 * During the execution the level can be manually changed by the function below.
 *
 * @see void changeLogLevel(LOG_LEVEL_CHANGE_ON typ, LOG_LEVEL level)
 */
static const quill::LogLevel DEFAULT_FILE_LOG_LEVEL = quill::LogLevel::Info;

#endif //SIM_TO_DUT_INTERFACE_DUTLOGGERCONFIG_H
