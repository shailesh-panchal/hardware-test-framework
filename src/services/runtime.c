#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


#include "runtime.h"
#include "safe_string.h"

/**
 * @brief Runtime framework context.
 *
 * This structure contains references to all active framework managers.
 * It provides a single access point for framework services during runtime.
 */
struct runtime_manager_t
{
    /**
     * Configuration Manager instance.
     */
    config_manager_t *config_manager;

    /**
     * Device Manager instance.
     */
    device_manager_t *device_manager;

    /**
     * Function Manager instance.
     */
    function_manager_t *function_manager;

    /**
     * Test Manager instance.
     */
    test_manager_t *test_manager;

    /**
     * Test Manager instance.
     */
    test_engine_t *test_engine;

};

runtime_manager_t* runtime_manager_init(const char *config_path){
    if(config_path == NULL)
        return NULL;

    runtime_manager_t *runtime = NULL;

    runtime = (runtime_manager_t *)malloc(sizeof(runtime_manager_t));

    if(runtime == NULL)
        return NULL;

    runtime->config_manager = config_manager_init(config_path);
    if(NULL == runtime->config_manager) {
        free(runtime);
        return NULL;
    }

    if(0 != config_manager_load(runtime->config_manager)) {
        printf("failed to load configuration\n");
        config_manager_deinit(runtime->config_manager);
        free(runtime);
        return NULL;
    }

    printf("Config Manager initialized with path: %s\n", config_path);

    runtime->device_manager = device_manager_init(runtime->config_manager);
    if(NULL == runtime->device_manager) {
        printf("failed to load configuration\n");
        config_manager_deinit(runtime->config_manager);
        free(runtime);
        return NULL;
    }

    runtime->function_manager = function_manager_init(runtime->config_manager,runtime->device_manager);
    if(NULL == runtime->function_manager) {
        printf("failed init the function manager\n");
        device_manager_deinit(runtime->device_manager);
        config_manager_deinit(runtime->config_manager);
        free(runtime);
        return NULL;
    }

    runtime->test_manager = test_manager_init(runtime->config_manager,runtime->function_manager);
    if(NULL == runtime->test_manager) {
        printf("failed to int the test manager\n");
        function_manager_deinit(runtime->function_manager); 
        device_manager_deinit(runtime->device_manager);
        config_manager_deinit(runtime->config_manager);
        free(runtime);
        return NULL;
    }

    runtime->test_engine = test_engine_init(runtime->test_manager,runtime->function_manager,runtime->device_manager);
    if(NULL == runtime->test_manager) {
        printf("failed to int the test manager\n");
        test_manager_deinit(runtime->test_manager);
        function_manager_deinit(runtime->function_manager);
        device_manager_deinit(runtime->device_manager);
        config_manager_deinit(runtime->config_manager);
        free(runtime);
        return NULL;
    }
    return runtime;
}

int32_t runtime_manager_deinit(runtime_manager_t *runtime){
    if(runtime == NULL)
        return -1;

    test_engine_deinit(runtime->test_engine);
    test_manager_deinit(runtime->test_manager);
    function_manager_deinit(runtime->function_manager);
    device_manager_deinit(runtime->device_manager);
    config_manager_deinit(runtime->config_manager);
    free(runtime);

    return 0;
}

config_manager_t* runtime_manager_get_config_manager(runtime_manager_t *runtime) {
    if(runtime == NULL)
        return NULL;

    return runtime->config_manager;
}
device_manager_t* runtime_manager_get_device_manager(runtime_manager_t *runtime) {
    if(runtime == NULL)
        return NULL;

    return runtime->device_manager;
}
function_manager_t* runtime_manager_get_function_manager(runtime_manager_t *runtime) {
    if(runtime == NULL)
        return NULL;

    return runtime->function_manager;
}
test_manager_t* runtime_manager_get_test_manager(runtime_manager_t *runtime) {
    if(runtime == NULL)
        return NULL;

    return runtime->test_manager;
}
test_engine_t* runtime_manager_get_test_engine(runtime_manager_t *runtime) {
    if(runtime == NULL)
        return NULL;

    return runtime->test_engine;
}