#ifndef PLATFORM_PARSER_H
#define PLATFORM_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>


#include "device.h"
#include "function.h"



#define MAX_BINDINGS 64
#define PLATFORM_NAME_LENGTH 64
#define PLATFORM_VENDOR_LENGTH 64
#define PLATFORM_SOC_LENGTH 64
#define PLATFORM_DESC_LENGTH 256
#define PLUGIN_NAME_LENGTH 64
#define INTERFACE_NAME_LENGTH 32
#define COMPATIBLE_NAME_LENGTH 64
#define MAX_CONFIG_PARAMETERS 32
#define CONFIG_KEY_LENGTH     64
#define CONFIG_STRING_LENGTH 128

typedef enum
{
    CONFIG_VALUE_STRING = 0,
    CONFIG_VALUE_INT,
    CONFIG_VALUE_DOUBLE,
    CONFIG_VALUE_BOOL

} config_value_type_t;

typedef struct
{
    char key[CONFIG_KEY_LENGTH];

    config_value_type_t type;

    union
    {
        char string_value[CONFIG_STRING_LENGTH];
        uint32_t int_value;
        double double_value;
        bool bool_value;
    } value;

} config_parameter_t;

typedef struct
{
    uint32_t parameter_count;

    config_parameter_t parameters[MAX_CONFIG_PARAMETERS];

} configuration_t;


typedef struct
{
    char device[DEVICE_NAME_LENGTH];

    char plugin[PLUGIN_NAME_LENGTH];

    char interface[INTERFACE_NAME_LENGTH];

    char compatible[COMPATIBLE_NAME_LENGTH];

    configuration_t configuration;

} device_binding_t;

typedef struct
{
    char name[PLATFORM_NAME_LENGTH];
    char vendor[PLATFORM_VENDOR_LENGTH];
    char soc[PLATFORM_SOC_LENGTH];
    char description[PLATFORM_DESC_LENGTH];

} platform_info_t;


// ============================================================================
// 1. THE SINGLE SOURCE OF TRUTH
// Add, remove, or reorder features here. The rest of the code updates itself!
// ============================================================================
#define EXPAND_ALL_FEATURES \
    X(wifi) \
    X(bluetooth) \
    X(ethernet) \
    X(gsm) \
    X(lte) \
    X(fiveg) \
    X(zigbee) \
    X(nfc) \
    X(gps) \
    X(serial) \
    X(camera) \
    X(audio) \
    X(hdmi) \
    X(isp) \
    X(gpu) \
    X(npu) \
    X(internal_usb) \
    X(external_usb) \
    X(pci) \
    X(rtc) \
    X(power_monitoring) \
    X(temperature_monitoring)

// ============================================================================
// 2. AUTOMATIC STRUCT GENERATION
// ============================================================================
// The preprocessor expands this into: bool wifi; bool bluetooth; ...
typedef struct {
#define X(name) bool name;
    EXPAND_ALL_FEATURES
#undef X
} platform_features_t;

typedef struct
{
    uint32_t schema_version;

    platform_info_t platform;

    platform_features_t features;

    uint32_t binding_count;

    device_binding_t bindings[MAX_BINDINGS];

} platform_config_t;


int32_t platform_parser_load(const char* filename,platform_config_t* cfg);
void platform_print(platform_config_t* cfg);

#endif // PLATFORM_PARSER_H