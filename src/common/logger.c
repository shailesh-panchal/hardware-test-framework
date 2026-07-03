#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"


#define LOGGER_MAX_QUEUE_SIZE     128
#define LOGGER_MAX_MESSAGE_SIZE   1024
#define LOGGER_TIMESTAMP_SIZE     32


typedef struct{
    char message[LOGGER_MAX_MESSAGE_SIZE];
} LoggerMessage_t;


typedef struct {
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} CircularFifo_t;

typedef struct {
    LogLevel_e logLevel;

    CircularFifo_t fifo;

    LoggerMessage_t queue[LOGGER_MAX_QUEUE_SIZE];

    pthread_mutex_t queueLock;
    pthread_cond_t queueCond;

    pthread_t workerThread;

    bool running;
} LoggerContext_t;

static LoggerContext_t loggerContext;

static const char* get_level_string(LogLevel_e level) {
    switch (level) {
        case LOG_LEVEL_TRACE: return "TRACE  ";
        case LOG_LEVEL_DEBUG: return "DEBUG  ";
        case LOG_LEVEL_INFO:  return "INFO   ";
        case LOG_LEVEL_WARN:  return "WARN   ";
        case LOG_LEVEL_ERROR: return "ERROR  ";
        case LOG_LEVEL_FATAL: return "FATAL  ";
        default:              return "UNKNOWN";
    }
}

static void logger_get_timestamp(char *buffer, size_t size) {
    time_t rawtime;
    struct tm timeinfo;

    time(&rawtime);
    #ifdef _WIN32
        localtime_s(&timeinfo, &rawtime);
    #else
        localtime_r(&rawtime, &timeinfo);
    #endif

   if (strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &timeinfo) == 0){
        buffer[0] = '\0';
    }
}

static int enqueue(const char *msg) {
    
    pthread_mutex_lock(&loggerContext.queueLock);
    
    if (loggerContext.fifo.count >= LOGGER_MAX_QUEUE_SIZE) {
        pthread_mutex_unlock(&loggerContext.queueLock);
        return -1;
    }

    snprintf(loggerContext.queue[loggerContext.fifo.tail].message, LOGGER_MAX_MESSAGE_SIZE, "%s",msg);

    loggerContext.fifo.tail = (loggerContext.fifo.tail + 1) % LOGGER_MAX_QUEUE_SIZE;
    loggerContext.fifo.count++;
    
    pthread_cond_signal(&loggerContext.queueCond);
    pthread_mutex_unlock(&loggerContext.queueLock);

    return 0;
}

static int dequeue(LoggerMessage_t *msg) {
    
    pthread_mutex_lock(&loggerContext.queueLock);
    
    while (loggerContext.fifo.count == 0 && loggerContext.running) {
        pthread_cond_wait(&loggerContext.queueCond, &loggerContext.queueLock);
    }

    if(loggerContext.fifo.count == 0 && !loggerContext.running) {
        pthread_mutex_unlock(&loggerContext.queueLock);
        return -1;
    }

    *msg = loggerContext.queue[loggerContext.fifo.head];
    loggerContext.fifo.head = (loggerContext.fifo.head + 1) % LOGGER_MAX_QUEUE_SIZE;
    loggerContext.fifo.count--;
    
    pthread_mutex_unlock(&loggerContext.queueLock);
    return 0;
}

static void* logger_worker(void* arg) {
    (void)arg;
    while (loggerContext.running || loggerContext.fifo.count > 0) {
        LoggerMessage_t msg;
        dequeue(&msg);
        printf("%s\n", msg.message);
        fflush(stdout);
    }

    return NULL;
}

int32_t logger_init(void) {
    
    pthread_mutex_init(&loggerContext.queueLock, NULL);

    pthread_cond_init(&loggerContext.queueCond, NULL);

    int ret = pthread_create(&loggerContext.workerThread, NULL,logger_worker, NULL);

    if(ret != 0) {
        pthread_mutex_destroy(&loggerContext.queueLock);
        pthread_cond_destroy(&loggerContext.queueCond);
        return -1;
    }
        
    loggerContext.logLevel = LOG_LEVEL_ERROR;
    loggerContext.running = true;
    loggerContext.fifo.head = 0;
    loggerContext.fifo.tail = 0;
    loggerContext.fifo.count = 0;
    return 0;
}

void logger_set_level(LogLevel_e level){
    loggerContext.logLevel = level;
}

LogLevel_e logger_get_level(void) {
    return loggerContext.logLevel;
}

char* logger_get_level_string(void) {
    switch (loggerContext.logLevel)
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


void logger_deinit(void) {
    pthread_mutex_lock(&loggerContext.queueLock);

    loggerContext.running = 0;

    pthread_cond_signal(&loggerContext.queueCond);

    pthread_mutex_unlock(&loggerContext.queueLock);

    pthread_join(
        loggerContext.workerThread,
        NULL);
}

void logger_log(LogLevel_e level,const char* file,const char* function,int line,const char* fmt,...) {
    if(!(level & loggerContext.logLevel)) {
        return;
    }
    if (file == NULL) {
        file = "UNKNOWN";
    }
    if (function == NULL) {
        function = "UNKNOWN";
    }

    if (fmt == NULL) {
        return;
    }
    char timestamp[LOGGER_TIMESTAMP_SIZE];
    char message[LOGGER_MAX_MESSAGE_SIZE - 300]; // Reserve space for timestamp, level, and file, function and line number information
    char finalMsg[LOGGER_MAX_MESSAGE_SIZE];
    va_list args;

    logger_get_timestamp(timestamp,sizeof(timestamp));

    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    snprintf(finalMsg, sizeof(finalMsg), "%s [%s] [%s:%d %s] %s", timestamp, get_level_string(level), file, line , function, message);
    enqueue(finalMsg);
}

