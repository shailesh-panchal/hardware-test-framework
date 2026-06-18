#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


#include "config-manager.h"
#include "platform.h"
#include "device.h"
#include "function.h"
#include "test.h"
#include "safe_string.h"


struct config_manager_t{
    
    char config_path[CONFIG_MANAGER_FILE_PATH];
    device_config_t device_config;
    function_config_t function_config;
    test_config_t test_config;
    platform_config_t platform_config;

};


config_manager_t* config_manager_init(const char *config_path){

    config_manager_t *cfg_man = NULL;

    if(config_path == NULL)
        return NULL;

    cfg_man = (config_manager_t *)malloc(sizeof(config_manager_t));

    if(cfg_man == NULL)
        return NULL;

    memset(cfg_man,0,sizeof(config_manager_t));

    safe_string_copy(cfg_man->config_path,config_path,sizeof(cfg_man->config_path));

    return cfg_man;
}

int32_t config_manager_load(config_manager_t *cfg_man) {
    if(cfg_man == NULL)
        return -1;
    
    char config_file_path[CONFIG_MANAGER_FILE_PATH] = {0};
    
    safe_sprintf(config_file_path, sizeof(config_file_path), "%s/%s",cfg_man->config_path,"device.jsonc");

    if(0 != device_parser_load(config_file_path, &cfg_man->device_config)) {
        printf( "Failed to load device configuration.\n");
        return -1;
    }

    memset(config_file_path,0,sizeof(config_file_path));
    safe_sprintf(config_file_path, sizeof(config_file_path),"%s/%s",cfg_man->config_path,"function.jsonc");

    if(0 != function_parser_load(config_file_path, &cfg_man->function_config)) {
        printf( "Failed to load function configuration.\n");
        return -1;
    }

    memset(config_file_path,0,sizeof(config_file_path));
    safe_sprintf(config_file_path, sizeof(config_file_path), "%s/%s",cfg_man->config_path,"test.jsonc");
    
    if(0 != test_parser_load(config_file_path, &cfg_man->test_config)) {
        printf( "Failed to load test configuration.\n");
        return -1;
    }
 
    memset(config_file_path,0,sizeof(config_file_path));
    safe_sprintf(config_file_path, sizeof(config_file_path), "%s/%s",cfg_man->config_path,"platform.json");
    
    if(0 != platform_parser_load(config_file_path, &cfg_man->platform_config)) {
        printf( "Failed to load platform configuration.\n");
        return -1;
    }
    return 0;
}

int32_t config_manager_print(config_manager_t *cfg_man) {
    if(cfg_man == NULL)
        return -1;

    
    device_printf(&cfg_man->device_config);
    function_print(&cfg_man->function_config);
    platform_print(&cfg_man->platform_config);
    test_printf(&cfg_man->test_config);

    return 0;
}

int32_t config_manager_platform_print(config_manager_t *cfg_man) {
    if(cfg_man == NULL)
        return -1;

    platform_print(&cfg_man->platform_config);

    return 0;
}


int32_t config_manager_deinit(config_manager_t *cfg_man){
    if(cfg_man == NULL)
        return -1;

    free(cfg_man);

    return 0;
}

int32_t config_manager_get_platform_info(config_manager_t *cfg_man, platform_info_t *platform_info) {
    if((cfg_man == NULL) || (platform_info == NULL))
        return -1;

    memcpy(platform_info,&cfg_man->platform_config.platform,sizeof(platform_info_t));

    return 0;
}

int32_t config_manager_get_platform_device_count(config_manager_t *cfg_man, uint32_t *device_count) {
    if((cfg_man == NULL) || (device_count == NULL))
        return -1;
    
    *device_count = cfg_man->platform_config.binding_count;
    return 0;
}

int32_t config_manager_get_platform_device_by_index(config_manager_t *cfg_man, uint32_t index, device_binding_t *device_binding) {
    if((cfg_man == NULL) || (device_binding == NULL))
        return -1;
    
    if(index > cfg_man->platform_config.binding_count)
        return -1;

    memcpy(device_binding, &cfg_man->platform_config.bindings[index], sizeof(device_binding_t));
    return 0;
}

int32_t config_manager_get_device_count(config_manager_t *cfg_man, uint32_t *device_count) {
    if((cfg_man == NULL) || (device_count == NULL))
        return -1;
    
    *device_count = cfg_man->device_config.count;
    return 0;
}
int32_t config_manager_get_device_by_index(config_manager_t *cfg_man, uint32_t index, device_def_t *device) {
    if((cfg_man == NULL) || (device == NULL))
        return -1;
    
    if(index > cfg_man->device_config.count)
        return -1;

    memcpy(device, &cfg_man->device_config.devices[index], sizeof(device_def_t));
    return 0;
}

int32_t config_manager_get_function_count(config_manager_t *cfg_man, uint32_t *count) {
    if((cfg_man == NULL) || (count == NULL))
        return -1;
    
    *count = cfg_man->function_config.count;
    return 0;
}

int32_t config_manager_get_function_by_index(config_manager_t *cfg_man, uint32_t index, function_def_t *function) {
    if((cfg_man == NULL) || (function == NULL))
        return -1;
    
    if(index > cfg_man->function_config.count)
        return -1;

    memcpy(function, &cfg_man->function_config.functions[index], sizeof(function_def_t));
    return 0;
}

int32_t config_manager_get_test_count(config_manager_t *cfg_man, uint32_t *count) {
    if((cfg_man == NULL) || (count == NULL))
        return -1;
    
    *count = cfg_man->test_config.count;
    return 0;
}
int32_t config_manager_get_test_by_index(config_manager_t *cfg_man, uint32_t index, test_def_t *test) {
    if((cfg_man == NULL) || (test == NULL))
        return -1;
    
    if(index > cfg_man->test_config.count)
        return -1;

    memcpy(test, &cfg_man->test_config.tests[index], sizeof(test_def_t));
    return 0;
}