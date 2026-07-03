#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "runtime.h"
#include "safe_string.h"

#undef LOG_MODULE
#define LOG_MODULE "main"
#include "logger.h"

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

    return LOG_LEVEL_INFO;
}

int32_t main(int argc, char *argv[]) {

    logger_init();

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--log-level") == 0)
        {
            int32_t ret = 0;
            while((i + 1) < argc)
            {
                ret |= parse_log_level(argv[i + 1]);
                i++;
            }
            logger_set_level(ret);
        }
    }

    if(argc != 2) {
        LOG_ERROR("Usage: %s <config_path>",argv[0]);
        return EXIT_FAILURE;
    }

    runtime_manager_t *runtime = NULL;
    char config_path[255] = {0};

    safe_string_copy(config_path, argv[1], sizeof(config_path));


    runtime = runtime_manager_init(config_path);
    if(NULL == runtime) {
        return EXIT_FAILURE;
    }

    runtime_manager_deinit(runtime);
    return EXIT_SUCCESS;
}
