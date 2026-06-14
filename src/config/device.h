#ifndef DEVICE_PARSER_H
#define DEVICE_PARSER_H

#include <stdint.h>

#define MAX_DEVICES 128
#define DEVICE_NAME_LENGTH 64
#define DEVICE_DESC_LENGTH 128

typedef enum
{
    DEVICE_TYPE_LED,
    DEVICE_TYPE_BUTTON,
    DEVICE_TYPE_TEMPERATURE_SENSOR,
    DEVICE_TYPE_POWER_SORCE,
    DEVICE_TYPE_ACCELEROMETER,
    DEVICE_TYPE_CAMERA,
    DEVICE_TYPE_NW_INTERFACE,
    DEVICE_TYPE_STORAGE,
    DEVICE_TYPE_AUDIO,
    DEVICE_TYPE_DISPLAY,
    DEVICE_TYPE_FAN,
    DEVICE_TYPE_BLUETOOTH,
    DEVICE_TYPE_NPU,
    DEVICE_TYPE_CONSOLE,
    DEVICE_TYPE_UNKNOWN

} DeviceType_e;


typedef struct
{
    char id[DEVICE_NAME_LENGTH];
    DeviceType_e type;
    char name[DEVICE_NAME_LENGTH];
    char description[DEVICE_DESC_LENGTH];

} device_def_t;


typedef struct
{
    uint32_t schema_version;
    device_def_t devices[MAX_DEVICES];
    uint32_t count;

} device_config_t;

int32_t device_parser_load(const char* filename,device_config_t* cfg);
void device_printf(device_config_t *cfg);
#endif // DEVICE_PARSER_H