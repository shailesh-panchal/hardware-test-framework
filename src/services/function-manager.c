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
        printf("failed to get the function count \n");
        return -1;
    }

    if(0 != device_manager_get_count(dm, &device_count)) {
        printf("failed to get the platform device count \n");
        return -1;
    }
    uint32_t fm_count = 0;
    for(uint32_t fun_index =0; fun_index < function_count; fun_index++) {
        if(0 != config_manager_get_function_by_index(cm,fun_index,&function_def)) {
            printf("failed  to getn function defination\n");
            continue;
        }

        for(uint32_t device_index=0; device_index < device_count; device_index++) {
            if(0 != device_manager_get_device_by_index(dm, device_index, &device)) {
                printf("failed to get the device information for device_index %d\n",device_index);
                continue;
            }
            if(safe_string_compare(device.name,function_def.name)) {
                safe_string_copy(fm->functions[fm_count].name,function_def.name,sizeof(fm->functions[fm_count].name));
                fm->functions[fm_count].available = 1;
                memcpy(&fm->functions[fm_count].definition,&function_def,sizeof(function_def));
                fm->count = fm_count;
                fm_count++;
            }
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