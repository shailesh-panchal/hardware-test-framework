#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LoggerContext_t g_logger;

static void* logger_worker(void* arg);
static int dequeue(LoggerMessage_t *msg);
static int enqueue(const char *msg);
int32_t logger_init(void)
{
    pthread_mutex_init(
        &g_logger.queueLock,
        NULL);

    pthread_cond_init(
        &g_logger.queueCond,
        NULL);

    int ret = pthread_create(
        &g_logger.workerThread,
        NULL,
        logger_worker,
        NULL);
        
    g_logger.logLevel = LOG_LEVEL_ERROR;
    g_logger.running = true;
    g_logger.fifo.head = 0;
    g_logger.fifo.tail = 0;
    g_logger.fifo.count = 0;
    return 0;
}

void logger_set_level(LogLevel_e level){ g_logger.logLevel = level; }

LogLevel_e logger_get_level(void){ return g_logger.logLevel; }

char* logger_get_level_string(void)
{
    switch (g_logger.logLevel)
    {
        case LOG_LEVEL_TRACE: return "TRACE";
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_FATAL: return "FATAL";
        default:              return "UNKNOWN";
    }
}

static const char* get_level_string(LogLevel_e level)
{
    switch (level)
    {
        case LOG_LEVEL_TRACE: return "TRACE  ";
        case LOG_LEVEL_DEBUG: return "DEBUG  ";
        case LOG_LEVEL_INFO:  return "INFO   ";
        case LOG_LEVEL_WARN:  return "WARN   ";
        case LOG_LEVEL_ERROR: return "ERROR  ";
        case LOG_LEVEL_FATAL: return "FATAL  ";
        default:              return "UNKNOWN";
    }
}

void logger_get_timestamp(char *buffer, size_t size)
{
    time_t rawtime;
    struct tm timeinfo;

    time(&rawtime);

    #ifdef _WIN32
        localtime_s(&timeinfo, &rawtime);
    #else
        localtime_r(&rawtime, &timeinfo);
    #endif

   if (strftime(
        buffer,
        size,
        "%Y-%m-%d %H:%M:%S",
        &timeinfo) == 0)
    {
        buffer[0] = '\0';
    }
}

static int enqueue(const char *msg)
{
    
    pthread_mutex_lock(&g_logger.queueLock);
    
    if (g_logger.fifo.count >= LOGGER_MAX_QUEUE_SIZE)
    {
        pthread_mutex_unlock(&g_logger.queueLock);
        return -1;
    }

    snprintf(
        g_logger.queue[g_logger.fifo.tail].message,
        LOGGER_MAX_MESSAGE_SIZE,
        "%s",
        msg);

    g_logger.fifo.tail = (g_logger.fifo.tail + 1) % LOGGER_MAX_QUEUE_SIZE;
    g_logger.fifo.count++;
    
    pthread_cond_signal(&g_logger.queueCond);
    pthread_mutex_unlock(&g_logger.queueLock);

    return 0;
}

static int dequeue(LoggerMessage_t *msg)
{
    
    pthread_mutex_lock(&g_logger.queueLock);
    
    while (g_logger.fifo.count == 0 && g_logger.running)
    {
        pthread_cond_wait(
            &g_logger.queueCond,
            &g_logger.queueLock);
    }

    if (g_logger.fifo.count == 0 && !g_logger.running)
    {
        pthread_mutex_unlock(&g_logger.queueLock);
        return -1;
    }

    *msg = g_logger.queue[g_logger.fifo.head];
    g_logger.fifo.head = (g_logger.fifo.head + 1) % LOGGER_MAX_QUEUE_SIZE;
    g_logger.fifo.count--;
    
    pthread_mutex_unlock(&g_logger.queueLock);

    return 0;
}

static void* logger_worker(void* arg)
{
    (void)arg;
    while (g_logger.running || g_logger.fifo.count > 0)
    {
        LoggerMessage_t msg;
        dequeue(&msg);
        printf("%s\n", msg.message);
        fflush(stdout);
    }

    return NULL;
}

void logger_deinit(void)
{
    pthread_mutex_lock(&g_logger.queueLock);

    g_logger.running = 0;

    pthread_cond_signal(&g_logger.queueCond);

    pthread_mutex_unlock(&g_logger.queueLock);

    pthread_join(
        g_logger.workerThread,
        NULL);
}

void logger_log(LogLevel_e level,const char* module,const char* fmt,...)
{
    if(!(level & g_logger.logLevel))
    {
        return;
    }
    if (module == NULL)
    {
        module = "UNKNOWN";
    }

    if (fmt == NULL)
    {
        return;
    }
    char timestamp[LOGGER_TIMESTAMP_SIZE];

    logger_get_timestamp(
        timestamp,
        sizeof(timestamp));

    char message[LOGGER_MAX_MESSAGE_SIZE];

    va_list args;

    va_start(args, fmt);

    vsnprintf(
        message,
        sizeof(message),
        fmt,
        args);

    va_end(args);

    char finalMsg[LOGGER_MAX_MESSAGE_SIZE];

    snprintf(
        finalMsg,
        sizeof(finalMsg),
        "%s [%s] [%s] %s",
        timestamp,
        get_level_string(level),
        module,
        message);

    enqueue(finalMsg);

}

