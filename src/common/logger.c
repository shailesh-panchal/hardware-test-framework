#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LogLevel_e g_logLevel = LOG_LEVEL_INFO;

int32_t logger_init(void)
{
    g_logLevel = LOG_LEVEL_ALL;
    return 0;
}

void logger_set_level(LogLevel_e level){ g_logLevel = level; }

LogLevel_e logger_get_level(void){ return g_logLevel; }

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

char* TimeStamp_get_current_time_string(void)
{
    static char buffer[20];
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

void logger_log(
    LogLevel_e level,
    const char* module,
    const char* fmt,
    ...)
{
    if(!(level & g_logLevel))
    {
        return;
    }

    printf("%s [%s] [%s] ",
            TimeStamp_get_current_time_string(),
           get_level_string(level),
           module);

    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

