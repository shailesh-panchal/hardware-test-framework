#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>


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

typedef struct
{
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} CircularFifo_t;

typedef struct
{
    LogLevel_e logLevel;

    CircularFifo_t fifo;

    LoggerMessage_t queue[LOGGER_MAX_QUEUE_SIZE];

    pthread_mutex_t queueLock;
    pthread_cond_t queueCond;

    pthread_t workerThread;

    bool running;

} LoggerContext_t;  


void logger_deinit(void);


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