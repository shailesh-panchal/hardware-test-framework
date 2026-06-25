#include <stdio.h>
#include <assert.h>
#include "../common/logger.h"
#include <string.h>
#include <unistd.h>
#undef LOG_MODULE
#define LOG_MODULE "DEVICE_MANAGER"

static LogLevel_e parse_log_level(
    const char *levelStr)
{
    if(strcmp(levelStr, "trace") == 0)
        return LOG_LEVEL_TRACE;

    if(strcmp(levelStr, "debug") == 0)
        return LOG_LEVEL_DEBUG;

    if(strcmp(levelStr, "info") == 0)
        return LOG_LEVEL_INFO;

    if(strcmp(levelStr, "warn") == 0)
        return LOG_LEVEL_WARN;

    if(strcmp(levelStr, "error") == 0)
        return LOG_LEVEL_ERROR;

    if(strcmp(levelStr, "fatal") == 0)
        return LOG_LEVEL_FATAL;

    if(strcmp(levelStr, "all") == 0)
        return LOG_LEVEL_ALL;

    return LOG_LEVEL_ERROR;
}

int main(int argc, char *argv[])
{
    
    
    int32_t ret = logger_init();


    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--log-level") == 0)
        {
            ret = 0;
            while((i + 1) < argc)
            {
                ret |= parse_log_level(argv[i + 1]);
                i++;
            }
            logger_set_level(ret);
        }
    }

    
    //Test 3: INFO Filtering\
    logger_set_level(LOG_LEVEL_INFO);
    for(int i = 0; i < 10; i++)
    {
        LOG_INFO("Message %d", i);
    }

    logger_deinit();
}