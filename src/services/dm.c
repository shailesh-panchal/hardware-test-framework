#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "dm.h"
#include "safe_string.h"


#define DEVICE_MANAGER_MAX_DEVICES    128

struct device_manager_t
{
    Device_t devices[DEVICE_MANAGER_MAX_DEVICES];

    uint32_t count;
};

static int32_t prepare_device_info(device_manager_t* dm, config_manager_t* config_manager) {
    if((config_manager == NULL) || (dm == NULL))
        return -1;

    uint32_t device_binding_count = 0, device_count = 0, dm_count = 0;

    device_binding_t device_binding = {0};
    device_def_t device_configuration = {0};

    if(0 != config_manager_get_platform_device_count(config_manager, &device_binding_count)) {
        printf("failed to get the platform device count \n");
        return -1;
    }

    if(device_binding_count == 0) {
        printf("platform do not contain any device\n");
        return -1;
    }

    if(0 != config_manager_get_device_count(config_manager, &device_count)) {
        printf("failed to get the device count \n");
        return -1;
    }

    for(uint32_t index=0; index < device_binding_count; index++) {
        if(0 != config_manager_get_platform_device_by_index(config_manager, index, &device_binding)) {
            printf("failed to get the platform device information for index %d\n",index);
            continue;
        }

        for(uint32_t device_index=0; device_index < device_count; device_index++) {
            if(0 != config_manager_get_device_by_index(config_manager, device_index, &device_configuration)) {
                printf("failed to get the device information for device_index %d\n",device_index);
                continue;
            }

            if(safe_string_compare(device_configuration.name,device_binding.device)) {
               //match the device and platform information. 
               //check that device is enabled in platform or not.
               if(device_binding.is_enabled) {
                    safe_string_copy(dm->devices[dm_count].name,device_configuration.name,sizeof(dm->devices[dm_count].name));
                    dm->devices[dm_count].state = DEVICE_STATE_REGISTERED;
                    memcpy(&dm->devices[dm_count].definition,&device_configuration,sizeof(device_def_t));
                    memcpy(&dm->devices[dm_count].binding,&device_binding,sizeof(device_binding_t));
                    dm->count = dm_count  + 1;
                    dm_count++;
               }
            }
        }
    }

    return 0;
}

device_manager_t* device_manager_init(config_manager_t* config_manager) {
    static bool init = 0;

    device_manager_t* dm = NULL;

    if(config_manager == NULL)
        return NULL;

    if(init)
        return NULL;

    dm = (device_manager_t *)malloc(sizeof(device_manager_t));

    if(dm == NULL)
        return NULL;

    if(0 != prepare_device_info(dm, config_manager)) {

        free(dm);
        return NULL;
    }

    init = 1;

    return dm;
}

int32_t device_manager_deinit(device_manager_t* dm) {
    if(dm == NULL)
        return -1;

    free(dm);

    return 0;
}

int32_t device_manager_get_count(device_manager_t* dm, uint32_t *count) {
    if(dm == NULL)
        return -1;

    *count =  dm->count;

    return 0;
}

int32_t device_manager_get_device_by_index(device_manager_t* dm, uint32_t index, Device_t *device) {
    if(dm == NULL)
        return -1;

    if(index > dm->count)
        return -1;

    memcpy(device, &dm->devices[index],sizeof(Device_t));

    return 0;
}

DeviceState_e device_manager_get_state(device_manager_t* dm, const char* name) {
    if((dm == NULL) || (name == NULL))
        return DEVICE_STATE_ERROR;

    for(uint32_t index=0; index < dm->count; index++) {
        if( safe_string_compare(name,dm->devices[index].name)) {
            return dm->devices[index].state;
        }
    }
    return DEVICE_STATE_UNKNOWN;
}


int32_t device_manager_enable(device_manager_t* dm, const char* name) {
    if((dm == NULL) || (name == NULL))
        return -1;

    for(uint32_t index=0; index < dm->count; index++) {
        if( safe_string_compare(name,dm->devices[index].name)) {
            dm->devices[index].state = DEVICE_STATE_ENABLED;
        }
    }
    return 0;
}

int32_t device_manager_disable(device_manager_t* dm, const char* name) {
    if((dm == NULL) || (name == NULL))
        return -1;

    for(uint32_t index=0; index < dm->count; index++) {
        if( safe_string_compare(name,dm->devices[index].name)) {
            dm->devices[index].state = DEVICE_STATE_DISABLED;
        }
    }
    return 0;
}