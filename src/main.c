#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "dm.h"
#include "config-manager.h"
#include "function-manager.h"

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

    config_manager_t *config_manager = NULL;
    device_manager_t *device_manager = NULL;
    function_manager_t *function_manager = NULL;

    config_manager = config_manager_init(argv[1]);

    if(config_manager == NULL)
        return EXIT_FAILURE;

    if(0 != config_manager_load(config_manager)) {
        LOG_ERROR("failed to load configuration");
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    LOG_TRACE("Config Manager initialized with path: %s\n", argv[1]);

    device_manager = device_manager_init(config_manager);
    if(device_manager == NULL) {
        LOG_ERROR("failed to load configuration");
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    uint32_t platform_device_count = 0, function_count = 0;
    
    if(0 != device_manager_get_count(device_manager,&platform_device_count)) {
        LOG_ERROR("failed to get the platform device count");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    LOG_DEBUG("Total Register platform device is %d", platform_device_count);
    //config_manager_platform_print(config_manager);

    function_manager = function_manager_init(config_manager,device_manager);
    if(function_manager == NULL) {
        LOG_ERROR("failed to initialize the function manager");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    if(0 != function_manager_get_count(function_manager,&function_count)) {
        LOG_ERROR("failed to get the platform device count");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);
        return EXIT_FAILURE;
    }

    LOG_DEBUG("Total functions is %d", function_count);

    function_def_t function_def = {0};

    if(0 != function_manager_get_function_by_index(function_manager,0,&function_def)){
        LOG_ERROR("failed to get the function information by index");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);   
        return EXIT_FAILURE;
    }

    function_manager_print(function_manager);

    device_manager_deinit(device_manager);
    config_manager_deinit(config_manager);
    function_manager_deinit(function_manager);
    return EXIT_SUCCESS;
}
