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

};

runtime_manager_t* runtime_manager_init(const char *config_path){
    if(config_path == NULL)
        return NULL;

    runtime_manager_t *runtime = NULL;

    runtime = (runtime_manager_t *)malloc(sizeof(runtime_manager_t));

    if(runtime == NULL)
        return NULL;

    
    config_manager_t *config_manager = NULL;
    device_manager_t *device_manager = NULL;
    function_manager_t *function_manager = NULL;
    test_manager_t *test_manager = NULL;

    config_manager = config_manager_init(config_path);

    if(config_manager == NULL) {
        free(runtime);
        return NULL;
    }

    if(0 != config_manager_load(config_manager)) {
        printf("failed to load configuration\n");
        config_manager_deinit(config_manager);
        free(runtime);
        return NULL;
    }

    printf("Config Manager initialized with path: %s\n", config_path);

    device_manager = device_manager_init(config_manager);
    if(device_manager == NULL) {
        printf("failed to load configuration\n");
        config_manager_deinit(config_manager);
        free(runtime);
        return NULL;
    }

    uint32_t platform_device_count = 0, function_count = 0, test_count = 0;
    
    if(0 != device_manager_get_count(device_manager,&platform_device_count)) {
        printf("failed to get the platform device count\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        free(runtime);
        return NULL;
    }

    printf("Totatl Register platform device is %d\n",platform_device_count);
    //config_manager_platform_print(config_manager);

    function_manager = function_manager_init(config_manager,device_manager);
    if(function_manager == NULL) {
        printf("failed init the function manager\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        free(runtime);
        return NULL;
    }

    if(0 != function_manager_get_count(function_manager,&function_count)) {
        printf("failed to get the platform device count\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);
        free(runtime);
        return NULL;
    }

    printf("Totatl function is %d\n",function_count);

    function_def_t function_def = {0};

    if(0 != function_manager_get_function_by_index(function_manager,0,&function_def)){
        printf("failed to get the function information by index\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);   
        free(runtime);
        return NULL;
    }

    test_manager = test_manager_init(config_manager,function_manager);
    if(test_manager == NULL) {
        printf("failed to int the test manager\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);   
        free(runtime);
        return NULL;
    }

    if(0 != test_manager_get_count(test_manager,&test_count)) {
        printf("failed to get the test count\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);
        test_manager_deinit(test_manager);
        free(runtime);
        return NULL;
    }

    printf("Total number of Test %d\n",test_count);

    runtime->device_manager = device_manager;
    runtime->config_manager = config_manager;
    runtime->function_manager = function_manager;
    runtime->test_manager = test_manager;

    return runtime;
}

int32_t runtime_manager_deinit(runtime_manager_t *runtime){
    if(runtime == NULL)
        return -1;
    device_manager_deinit(runtime->device_manager);
    config_manager_deinit(runtime->config_manager);
    function_manager_deinit(runtime->function_manager);
    test_manager_deinit(runtime->test_manager);

    free(runtime);

    return 0;
}