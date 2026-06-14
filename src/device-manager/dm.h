#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>


#include "device.h"
#include "platform.h"

typedef enum
{
    DEVICE_STATE_UNKNOWN = 0,
    DEVICE_STATE_REGISTERED,
    DEVICE_STATE_ENABLED,
    DEVICE_STATE_DISABLED,
    DEVICE_STATE_ERROR

} DeviceState_e;

typedef struct Device_t Device_t;

Device_t* device_manager_init(const device_config_t* device_cfg,const platform_config_t* platform_cfg);

int32_t device_manager_deinit(Device_t* device);

int32_t device_manager_validate(Device_t *device);

int32_t device_manager_get(Device_t* device, const char* name);

int32_t device_manager_get_count(Device_t* device);

DeviceState_e device_manager_get_state(Device_t *device, const char* name);

int32_t device_manager_enable(Device_t* device, const char* name);

int32_t device_manager_disable(Device_t* device, const char* name);

#endif // DEVICE_MANAGER_H