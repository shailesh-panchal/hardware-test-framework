#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "function-manager.h"
#include "safe_string.h"

#define LOG_MODULE "FUNCTION_MANAGER"
#include "logger.h"


struct function_manager_t
{
    Function_t functions[MAX_FUNCTIONS];

    uint32_t count;

};

static int32_t prepare_function_info(function_manager_t* fm, config_manager_t* cm, device_manager_t* dm){
    if((fm == NULL) || (cm == NULL) || (dm == NULL))
        return -1;

    uint32_t function_count = 0;
    uint32_t device_count = 0;

    function_def_t function_def = {0};
    Device_t device = {0};

    if(0 != config_manager_get_function_count(cm, &function_count)) {
        // printf("failed to get the function count \n");
        LOG_ERROR("failed to get the function count");
        return -1;
    }

    if(0 != device_manager_get_count(dm, &device_count)) {
        // printf("failed to get the platform device count \n");
        LOG_ERROR("failed to get the platform device count");

        return -1;
    }
    uint32_t fm_count = 0;
    bool match_found = 0;
    uint32_t total_match_found = 0;
    for(uint32_t fun_index =0; fun_index < function_count; fun_index++) {
        if(0 != config_manager_get_function_by_index(cm,fun_index,&function_def)) {
            // printf("failed  to getn function defination\n");
            LOG_ERROR("failed to get function definition");
            continue;
        }
        if(function_def.count == 0)
            continue;

        total_match_found = 0;
        //loop of the device which is required for function.
        for(uint8_t temp_index=0; temp_index < function_def.count; temp_index++){
            match_found = 0;
            for(uint32_t device_index=0; device_index < device_count; device_index++) {

                if(0 != device_manager_get_device_by_index(dm, device_index, &device)) {
                    // printf("failed to get the device information for device_index %d\n",device_index);
                    LOG_DEBUG("failed to get the device information for device_index %d", device_index);
                    continue;
                }
                if(safe_string_compare(device.name,function_def.device_name[temp_index].name)) {
                    match_found = 1; 
                    break;
                }
            }
            //break loop as device name not match with function name
            if(match_found == 0)
                break;
            else
                total_match_found++;
        }
        if(total_match_found == function_def.count) {
            //mark presence of device 
            fm->functions[fm_count].available = 1;
            safe_string_copy(fm->functions[fm_count].name,function_def.name,sizeof(fm->functions[fm_count].name));
            memcpy(&fm->functions[fm_count].definition,&function_def,sizeof(function_def_t));
            fm->count = fm_count + 1;
            fm_count++;
        }
    }

    return 0;
}

function_manager_t* function_manager_init(config_manager_t* config_manager,device_manager_t* device_manager){
    if((config_manager == NULL) || (device_manager == NULL))
        return NULL;

    function_manager_t* fm = NULL;

    fm = (function_manager_t*)malloc(sizeof(function_manager_t));

    if(fm == NULL)
        return NULL;

    if(0 != prepare_function_info(fm,config_manager,device_manager)) {
        free(fm);
        return NULL;
    }

    return fm;
}

int32_t function_manager_deinit(function_manager_t* fm) {
    if(fm == NULL)
        return -1;

    free(fm);

    return 0;
}

int32_t function_manager_get_count(function_manager_t* fm, uint32_t *count){
    if(fm == NULL)
        return -1;

    *count = fm->count;

    return 0;
}

int32_t function_manager_get_function(function_manager_t* fm, const char* name, function_def_t* function){
    if((fm == NULL) || (function == NULL) || (name == NULL))
        return -1;

    for (uint32_t index =0; index < fm->count; index++){
        if(safe_string_compare(name,fm->functions[index].name)) {
            memcpy(function, &fm->functions[index],sizeof(function_def_t));
            return 1;
        }
    }
    return -1;
}

int32_t function_manager_get_function_by_index(function_manager_t* fm, uint32_t index, function_def_t* function){
    if((fm == NULL) || (function == NULL))
        return -1;
    if(index >= fm->count) {
        return -1;
    }
    
    memcpy(function, &fm->functions[index].definition,sizeof(function_def_t));
    return 0;
}

bool function_manager_is_available(function_manager_t* fm, const char* name) {
    if((fm == NULL)  || (name == NULL))
        return 0;

    for (uint32_t index =0; index < fm->count; index++){
        if(safe_string_compare(name,fm->functions[index].name)) {
            return fm->functions[index].available;
        }
    }

    return 0;
}

int32_t function_manager_get_device_count(function_manager_t* fm, const char* name, uint32_t* count){
    if((fm == NULL) || (count == NULL) || (name == NULL))
        return -1;

    for (uint32_t index =0; index < fm->count; index++){
        if(safe_string_compare(name,fm->functions[index].name)) {
            *count = fm->functions[index].definition.count;
            return 1;
        }
    }
    return 0;
}

int32_t function_manager_get_device_by_index(function_manager_t* fm, char *name,uint32_t device_index, device_name_t* device_name){
    if((fm == NULL) || (device_name == NULL) || (name == NULL))
        return -1;

    for (uint32_t index =0; index < fm->count; index++){
        if(safe_string_compare(name,fm->functions[index].name)) {
            if(device_index < fm->functions[index].definition.count) {
                memcpy(device_name,&fm->functions[index].definition.device_name[device_index],sizeof(device_name_t));
                return 0;
            }
        }
    }
    return -1;
}

int32_t function_manager_print(function_manager_t* fm) {
    if(fm == NULL)
        return -1;

    // printf("Total available function count %d\n",fm->count);
    LOG_INFO("Total available function count %d", fm->count);

    for(uint16_t function_count =0; function_count < fm->count; function_count++) {
        // printf("name: %s\n",fm->functions[function_count].name);
        // printf("available: %d\n",fm->functions[function_count].available);
        // printf("device descriptor %s\n",fm->functions[function_count].definition.description);

        LOG_INFO("name: %s", fm->functions[function_count].name);
        LOG_INFO("available: %d", fm->functions[function_count].available);
        LOG_INFO("device descriptor %s", fm->functions[function_count].definition.description);
        
        // printf("device name %s\n",fm->functions[function_count].definition.name);
        LOG_INFO("device name %s", fm->functions[function_count].definition.name);
        for(uint16_t device_count = 0; device_count < fm->functions[function_count].definition.count; device_count++) {
            // printf("device name %s\n",fm->functions[function_count].definition.device_name[device_count].name);
            LOG_INFO("device name %s", fm->functions[function_count].definition.device_name[device_count].name);
        }
    }

    return 0;
}
