#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "dm.h"
#include "config-manager.h"
#include "function-manager.h"


int32_t main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("Usage: %s <config_path>\n",argv[0]);
        return EXIT_FAILURE;
    }

    config_manager_t *config_manager = NULL;
    device_manager_t *device_manager = NULL;
    function_manager_t *function_manager = NULL;

    config_manager = config_manager_init(argv[1]);

    if(config_manager == NULL)
        return EXIT_FAILURE;

    if(0 != config_manager_load(config_manager)) {
        printf("failed to load configuration\n");
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    printf("Config Manager initialized with path: %s\n", argv[1]);

    device_manager = device_manager_init(config_manager);
    if(device_manager == NULL) {
        printf("failed to load configuration\n");
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    uint32_t platform_device_count = 0, function_count = 0;
    
    if(0 != device_manager_get_count(device_manager,&platform_device_count)) {
        printf("failed to get the platform device count\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    printf("Totatl Register platform device is %d\n",platform_device_count);
    //config_manager_platform_print(config_manager);

    function_manager = function_manager_init(config_manager,device_manager);
    if(function_manager == NULL) {
        printf("failed init the function manager\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    if(0 != function_manager_get_count(function_manager,&function_count)) {
        printf("failed to get the platform device count\n");
        device_manager_deinit(device_manager);
        config_manager_deinit(config_manager);
        function_manager_deinit(function_manager);
        return EXIT_FAILURE;
    }

    printf("Totatl function is %d\n",function_count);

    function_def_t function_def = {0};

    if(0 != function_manager_get_function_by_index(function_manager,0,&function_def)){
        printf("failed to get the function information by index\n");
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
