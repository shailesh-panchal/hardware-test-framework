#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "cJSON.h"

#include "function.h"
#include "util.h"
#include "safe_string.h"

#define FUNCTION_JSON_SCHEMA_VERSION 1
#define FUNCTION_JSON_KEY_SCHEMA_VERSION "schema_version"
#define FUNCTION_JSON_KEY_FUNCTIONS "functions"
#define FUNCTION_JSON_KEY_NAME "name"
#define FUNCTION_JSON_KEY_DESC "description"
#define FUNCTION_JSON_KEY_DEVICE_NAME "device-name"

#undef LOG_MODULE
#define LOG_MODULE "FUNCTIONS_CONFIG"
#include "logger.h"

void function_print(function_config_t* cfg) {

    if (cfg == NULL)
        return;
    
    LOG_INFO("Schema Version: %d", cfg->schema_version);

    LOG_INFO("Total Functions Loaded: %d", cfg->count);
    LOG_INFO("===============================");

    for(uint32_t i = 0; i < cfg->count; i++) {
        LOG_INFO("Function Found:");
        LOG_INFO("  Name: %s", cfg->functions[i].name);
        LOG_INFO("  Description: %s", cfg->functions[i].description);
        LOG_INFO("  Device Names:");
        for(uint32_t j = 0; j < cfg->functions[i].count; j++) {
            LOG_INFO("    - %s", cfg->functions[i].device_name[j].name);
        }
        LOG_INFO("-------------------------------");
    }
    
}

int32_t function_parser_load(const char* filename,function_config_t* cfg){

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
            LOG_DEBUG("Error parsing JSON before: %s", error_ptr);
        }
        return -1;
    }

    // 3. Extract schema version
    cJSON *schema = cJSON_GetObjectItemCaseSensitive(root, FUNCTION_JSON_KEY_SCHEMA_VERSION);
    if (cJSON_IsNumber(schema)) {
        cfg->schema_version = schema->valueint;
    }

    cfg->count = 0; // Initialize function count
    cJSON *function = NULL;
    // 4. Step into the "functions" array
    cJSON *functions = cJSON_GetObjectItemCaseSensitive(root, FUNCTION_JSON_KEY_FUNCTIONS);
    if (cJSON_IsArray(functions)) {
        
        // 4. Iterate through every object inside the array
        cJSON_ArrayForEach(function, functions) {

            // Extract attributes safely using case-sensitive checks
            cJSON *name = cJSON_GetObjectItemCaseSensitive(function, FUNCTION_JSON_KEY_NAME);

            if (cJSON_IsString(name) && name->valuestring != NULL) {
                original_len = safe_string_copy(cfg->functions[cfg->count].name, 
                    name->valuestring, sizeof(cfg->functions[cfg->count].name));

                // Truncation detection metric
                if (original_len >= sizeof(cfg->functions[cfg->count].name)) {
                    LOG_WARN("String was truncated! Original length was %zu bytes.", original_len);
                }
            }

            cJSON *desc = cJSON_GetObjectItemCaseSensitive(function, FUNCTION_JSON_KEY_DESC);
            if (cJSON_IsString(desc) && desc->valuestring != NULL) {
                original_len = safe_string_copy(cfg->functions[cfg->count].description, 
                    desc->valuestring, sizeof(cfg->functions[cfg->count].description));

                // Truncation detection metric
                if (original_len >= sizeof(cfg->functions[cfg->count].description)) {
                    LOG_WARN("String was truncated! Original length was %zu bytes.", original_len);
                }
            }

            cJSON *device_names = cJSON_GetObjectItemCaseSensitive(function, FUNCTION_JSON_KEY_DEVICE_NAME);
            if (cJSON_IsArray(device_names)) {
                cJSON *device = NULL;
                int device_count = 0;
                cJSON_ArrayForEach(device, device_names) {
                    if (cJSON_IsString(device) && device->valuestring != NULL && device_count < MAX_DEVICES) {
                        original_len = safe_string_copy(cfg->functions[cfg->count].device_name[device_count].name, 
                            device->valuestring, sizeof(cfg->functions[cfg->count].device_name[device_count].name));
                        device_count++; //update the device count after processing each device name
                    }
                }
                cfg->functions[cfg->count].count = device_count;
            }  
            cfg->count++; //update the function count after processing each function
        }
    }
    cJSON_Delete(root);
    return 0;
}
