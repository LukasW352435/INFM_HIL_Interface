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
 * This constant defines the maximum number of files in a logging directory. If there are more
 * logfiles under the underlying path, old ones will be deleted. (like a date based file rotation)
 */
static const int FILE_BACKUP_COUNT = 10;

/**
 * This constant stores the default level for console logging.
 */
static const quill::LogLevel DEFAULT_CONSOLE_LOG_LEVEL = quill::LogLevel::Info;

/**
 * This constant stores the default level for console file logging.
 */
static const quill::LogLevel DEFAULT_FILE_LOG_LEVEL = quill::LogLevel::Info;

#endif //SIM_TO_DUT_INTERFACE_DUTLOGGERCONFIG_H
