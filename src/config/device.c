#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "cJSON.h"

#include "device.h"
#include "util.h"
#include "safe_string.h"

#define LOG_MODULE "DEVICE_MANAGEMENT"
#include "logger.h"

#define DEVICE_JSON_SCHEMA_VERSION 1
#define DEVICE_JSON_KEY_SCHEMA_VERSION "schema_version"
#define DEVICE_JSON_KEY_DEVICES "devices"   
#define DEVICE_JSON_KEY_ID "id"
#define DEVICE_JSON_KEY_TYPE "type"
#define DEVICE_JSON_KEY_NAME "name"
#define DEVICE_JSON_KEY_DESC "description"

static const char* device_type_to_string(DeviceType_e type) {
    switch (type) {
        case DEVICE_TYPE_LED: return "LED";
        case DEVICE_TYPE_BUTTON: return "Button";
        case DEVICE_TYPE_TEMPERATURE_SENSOR: return "Temperature Sensor";
        case DEVICE_TYPE_POWER_SORCE: return "Power Source";
        case DEVICE_TYPE_ACCELEROMETER: return "Accelerometer";
        case DEVICE_TYPE_CAMERA: return "Camera";
        case DEVICE_TYPE_NW_INTERFACE: return "Network Interface";
        case DEVICE_TYPE_STORAGE: return "Storage";
        case DEVICE_TYPE_AUDIO: return "Audio Device";
        case DEVICE_TYPE_DISPLAY: return "Display";
        case DEVICE_TYPE_FAN: return "Fan";
        case DEVICE_TYPE_BLUETOOTH: return "Bluetooth Device";
        case DEVICE_TYPE_NPU: return "NPU";
        case DEVICE_TYPE_CONSOLE: return "Console";
        default: return "Unknown";
    }
}

static DeviceType_e string_to_device_type(const char* type_str) {
    if (strcmp(type_str, "LED") == 0) return DEVICE_TYPE_LED;
    if (strcmp(type_str, "Button") == 0) return DEVICE_TYPE_BUTTON;
    if (strcmp(type_str, "Temperature Sensor") == 0) return DEVICE_TYPE_TEMPERATURE_SENSOR;
    if (strcmp(type_str, "Power Source") == 0) return DEVICE_TYPE_POWER_SORCE;
    if (strcmp(type_str, "Accelerometer") == 0) return DEVICE_TYPE_ACCELEROMETER;
    if (strcmp(type_str, "Camera") == 0) return DEVICE_TYPE_CAMERA;
    if (strcmp(type_str, "Network Interface") == 0) return DEVICE_TYPE_NW_INTERFACE;
    if (strcmp(type_str, "Storage") == 0) return DEVICE_TYPE_STORAGE;
    if (strcmp(type_str, "Audio Device") == 0) return DEVICE_TYPE_AUDIO;
    if (strcmp(type_str, "Display") == 0) return DEVICE_TYPE_DISPLAY;
    if (strcmp(type_str, "Fan") == 0) return DEVICE_TYPE_FAN;
    if (strcmp(type_str, "Bluetooth Device") == 0) return DEVICE_TYPE_BLUETOOTH;
    if (strcmp(type_str, "NPU") == 0) return DEVICE_TYPE_NPU;
    if (strcmp(type_str, "Console") == 0) return DEVICE_TYPE_CONSOLE;
    return DEVICE_TYPE_UNKNOWN;
}

int32_t device_parser_load(const char* filename,device_config_t* cfg){
    if (filename == NULL || cfg == NULL) {
        return -1;
    }

    size_t original_len = 0;

    char* json_str = read_file_to_string(filename);
    if (json_str == NULL) {
        return -1;
    }

     // 2. Parse the raw string into a cJSON tree structure
    cJSON *root = cJSON_Parse(json_str);
    
    // The raw string buffer is no longer needed after parsing
    free(json_str); 

    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            LOG_DEBUG("Error parsing JSON before: %s\n", error_ptr);
        }
        return -1;
    }

    // 3. Extract schema version
    cJSON *schema = cJSON_GetObjectItemCaseSensitive(root, DEVICE_JSON_KEY_SCHEMA_VERSION);
    if (cJSON_IsNumber(schema)) {
        cfg->schema_version = schema->valueint;
    }

    cfg->count = 0; // Initialize device count
    // 4. Step into the "devices" array
    cJSON *devices = cJSON_GetObjectItemCaseSensitive(root, DEVICE_JSON_KEY_DEVICES);
    if (cJSON_IsArray(devices)) {
        cJSON *device = NULL;

        // 4. Iterate through every object inside the array
        cJSON_ArrayForEach(device, devices) {
            // Extract attributes safely using case-sensitive checks
            cJSON *id   = cJSON_GetObjectItemCaseSensitive(device, DEVICE_JSON_KEY_ID);

            if (cJSON_IsString(id) && id->valuestring != NULL) {
                original_len = safe_string_copy(cfg->devices[cfg->count].id, 
                    id->valuestring, sizeof(cfg->devices[cfg->count].id));

                // Truncation detection metric
                if (original_len >= sizeof(cfg->devices[cfg->count].id)) {
                    LOG_WARN("String was truncated! Original length was %zu bytes.\n", original_len);
                }
            }

            cJSON *name = cJSON_GetObjectItemCaseSensitive(device, DEVICE_JSON_KEY_NAME);
            if (cJSON_IsString(name) && name->valuestring != NULL) {
                original_len = safe_string_copy(cfg->devices[cfg->count].name,
                     name->valuestring, sizeof(cfg->devices[cfg->count].name));
                if (original_len >= sizeof(cfg->devices[cfg->count].name)) {
                    LOG_WARN("String was truncated! Original length was %zu bytes.\n", original_len);
                }
            }

            cJSON *type = cJSON_GetObjectItemCaseSensitive(device, DEVICE_JSON_KEY_TYPE);
            if (cJSON_IsString(type) && type->valuestring != NULL) {
                cfg->devices[cfg->count].type = string_to_device_type(type->valuestring);
            }

            cJSON *desc = cJSON_GetObjectItemCaseSensitive(device, DEVICE_JSON_KEY_DESC);
            if (cJSON_IsString(desc) && desc->valuestring != NULL) {
                original_len = safe_string_copy(cfg->devices[cfg->count].description, 
                    desc->valuestring, sizeof(cfg->devices[cfg->count].description));
                if (original_len >= sizeof(cfg->devices[cfg->count].description)) {
                    LOG_WARN("String was truncated! Original length was %zu bytes.\n", original_len);
                }
            }
            cfg->count++;
        }
    }
    cJSON_Delete(root);
    return 0;
}

void device_printf(device_config_t *cfg) {

    if(cfg == NULL)
        return;

    LOG_INFO("Schema Version: %d\n", cfg->schema_version);
    LOG_INFO("Total devices parsed: %d\n", cfg->count);
    LOG_INFO("================================\n");
    for(uint32_t i = 0; i < cfg->count; i++) {
        LOG_INFO("Device %d:\n", i + 1);
        LOG_INFO("  ID: %s\n", cfg->devices[i].id);
        LOG_INFO("  Name: %s\n", cfg->devices[i].name);
        LOG_INFO("  Type: %s\n", device_type_to_string(cfg->devices[i].type));
        LOG_INFO("  Description: %s\n", cfg->devices[i].description);
        LOG_INFO("-------------------------------\n");
    }
}