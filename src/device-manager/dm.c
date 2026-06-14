#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "dm.h"

struct Device_t
{
    char name[DEVICE_NAME_LENGTH];

    DeviceState_e state;

    const device_def_t* definition;

    const device_binding_t* binding;

};

Device_t* device_manager_init(const device_config_t* device_cfg,const platform_config_t* platform_cfg) {
    static bool init = 0;

    Device_t* device = NULL;

    if((device_cfg == NULL) || (platform_cfg == NULL))
        return NULL;

    if(init)
        return NULL;

    device = (Device_t *)malloc(sizeof(Device_t));

    if(device == NULL)
        return NULL;

    init = 1;

    return device;
}

int32_t device_manager_deinit(Device_t* device) {
    if(device == NULL)
        return -1;

    free(device);

    return 0;
}

int32_t device_manager_validate(Device_t *device) {
    if(device == NULL)
        return -1;

    return 0;
}

int32_t device_manager_get(Device_t* device, const char* name) {
    if((device == NULL) || (name == NULL))
        return -1;

    return 0;
}

int32_t device_manager_get_count(Device_t* device) {
    if(device == NULL)
        return -1;

    return 0;
}

DeviceState_e device_manager_get_state(Device_t* device, const char* name) {
    if((device == NULL) || (name == NULL))
        return DEVICE_STATE_ERROR;

    return DEVICE_STATE_REGISTERED;
}


int32_t device_manager_enable(Device_t* device, const char* name) {
    if((device == NULL) || (name == NULL))
        return -1;

    return 0;
}

int32_t device_manager_disable(Device_t* device, const char* name) {
    if((device == NULL) || (name == NULL))
        return -1;

    return 0;
}