#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>


#ifndef LOG_MODULE
#define LOG_MODULE "UNKNOWN"
#endif

#define LOGGER_MAX_QUEUE_SIZE 128
#define LOGGER_MAX_MESSAGE_SIZE 1024
#define LOGGER_TIMESTAMP_SIZE 32
typedef struct
{
    char message[LOGGER_MAX_MESSAGE_SIZE];
} LoggerMessage_t;

static LoggerMessage_t g_queue[LOGGER_MAX_QUEUE_SIZE];

static uint32_t g_head = 0;
static uint32_t g_tail = 0;
static uint32_t g_count = 0;

static pthread_mutex_t g_queueMutex;
static pthread_cond_t  g_queueCond;

static pthread_t g_loggerThread;

static volatile int g_running = 1;
void logger_deinit(void);
typedef enum
{
    LOG_LEVEL_TRACE = 0x01,
    LOG_LEVEL_DEBUG = 0x02,
    LOG_LEVEL_INFO = 0x04,
    LOG_LEVEL_WARN = 0x08,
    LOG_LEVEL_ERROR = 0x10,
    LOG_LEVEL_FATAL = 0x20,
    LOG_LEVEL_ALL = 0x3F,
} LogLevel_e;

int32_t logger_init(void);

void logger_set_level(LogLevel_e level);

#define LOG_TRACE(...) \
    logger_log(LOG_LEVEL_TRACE, LOG_MODULE, __VA_ARGS__)

#define LOG_DEBUG(...) \
    logger_log(LOG_LEVEL_DEBUG, LOG_MODULE, __VA_ARGS__)

#define LOG_INFO(...) \
    logger_log(LOG_LEVEL_INFO, LOG_MODULE, __VA_ARGS__)

#define LOG_WARN(...) \
    logger_log(LOG_LEVEL_WARN, LOG_MODULE, __VA_ARGS__)

#define LOG_ERROR(...) \
    logger_log(LOG_LEVEL_ERROR, LOG_MODULE, __VA_ARGS__)

#define LOG_FATAL(...) \
    logger_log(LOG_LEVEL_FATAL, LOG_MODULE, __VA_ARGS__)


LogLevel_e logger_get_level(void);

void logger_log(
    LogLevel_e level,
    const char* module,
    const char* fmt,
    ...);

void logger_get_timestamp(
    char *buffer,
    size_t size);

#endif