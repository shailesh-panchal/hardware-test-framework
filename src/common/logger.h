/**
 * @file logger.h
 *
 * @brief Asynchronous logging interface.
 *
 * Provides thread-safe logging services for the
 * Hardware Test Framework.
 *
 * Features:
 * - Thread-safe logging
 * - Asynchronous message queue
 * - Multiple log levels
 * - Timestamp support
 * - Source file/function/line information
 * - Configurable log filtering
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Log TRACE message.
 *
 * Automatically captures source file,
 * function name and line number.
 */
#define LOG_TRACE(...) \
    logger_log(LOG_LEVEL_TRACE, __FILE__, __func__, __LINE__, __VA_ARGS__)

/**
 * @brief Log DEBUG message.
 *
 * Automatically captures source file,
 * function name and line number.
 */
#define LOG_DEBUG(...) \
    logger_log(LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__)

/**
 * @brief Log INFO message.
 *
 * Automatically captures source file,
 * function name and line number.
 */
#define LOG_INFO(...) \
    logger_log(LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)

/**
 * @brief Log WARNING message.
 *
 * Automatically captures source file,
 * function name and line number.
 */
#define LOG_WARN(...) \
    logger_log(LOG_LEVEL_WARN, __FILE__, __func__, __LINE__, __VA_ARGS__)

/**
 * @brief Log ERROR message.
 *
 * Automatically captures source file,
 * function name and line number.
 */
#define LOG_ERROR(...) \
    logger_log(LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)

/**
 * @brief Log FATAL message.
 *
 * Automatically captures source file,
 * function name and line number.
 */
#define LOG_FATAL(...) \
    logger_log(LOG_LEVEL_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)

/**
 * @brief Log severity levels.
 *
 * Log levels are represented as bit masks,
 * allowing multiple levels to be enabled
 * simultaneously.
 */
typedef enum
{
    LOG_LEVEL_TRACE = 0x01, /**< Trace information */
    LOG_LEVEL_DEBUG = 0x02, /**< Debug information */
    LOG_LEVEL_INFO  = 0x04, /**< General information */
    LOG_LEVEL_WARN  = 0x08, /**< Warning message */
    LOG_LEVEL_ERROR = 0x10, /**< Error message */
    LOG_LEVEL_FATAL = 0x20, /**< Fatal error */
    LOG_LEVEL_ALL   = 0x3F  /**< Enable all log levels */

} LogLevel_e;

/**
 * @brief Initialize logger.
 *
 * Creates the internal logging queue and
 * starts the logging worker thread.
 *
 * This function must be called before
 * any logging operation.
 *
 * @return Status code.
 *
 * @retval 0
 * Logger initialized successfully.
 *
 * @retval -1
 * Initialization failed.
 */
int32_t logger_init(void);

/**
 * @brief Deinitialize logger.
 *
 * Stops the logging worker thread,
 * flushes any pending log messages
 * and releases allocated resources.
 */
void logger_deinit(void);

/**
 * @brief Set active log level.
 *
 * Messages whose level is not enabled
 * will be discarded.
 *
 * @param[in] level
 * Log level mask.
 */
void logger_set_level(
        LogLevel_e level);

/**
 * @brief Get current log level.
 *
 * @return Current log level mask.
 */
LogLevel_e logger_get_level(void);

/**
 * @brief Log formatted message.
 *
 * Adds a formatted message to the
 * asynchronous logging queue.
 *
 * The message includes:
 * - Timestamp
 * - Log level
 * - Source file
 * - Function name
 * - Line number
 *
 * Normally this API should not be called
 * directly. Instead, use one of the
 * LOG_TRACE(), LOG_DEBUG(), LOG_INFO(),
 * LOG_WARN(), LOG_ERROR() or LOG_FATAL()
 * macros.
 *
 * @param[in] level
 * Log severity.
 *
 * @param[in] file
 * Source filename.
 *
 * @param[in] function
 * Function name.
 *
 * @param[in] line
 * Source line number.
 *
 * @param[in] fmt
 * printf-style format string.
 *
 * @param[in] ...
 * Variable argument list.
 */
void logger_log(
        LogLevel_e level,
        const char *file,
        const char *function,
        int line,
        const char *fmt,
        ...);

#endif /* LOGGER_H */