#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "cJSON.h"



#include "platform.h"
#include "util.h"
#include "safe_string.h"
#include "parser.h"

#undef LOG_MODULE
#define LOG_MODULE "PLATFORM_MANAGER"
#include "logger.h"

#define PLATFORM_JSON_SCHEMA_VERSION 1
#define PLATFORM_JSON_KEY_SCHEMA_VERSION "schema_version"
#define PLATFORM_JSON_KEY_PLATFORM "platform"
#define PLATFORM_JSON_KEY_FEATURES "features"
#define PLATFORM_JSON_KEY_BINDINGS "bindings"

static int32_t get_gpio(cJSON *config, uint32_t binding_count, platform_config_t *cfg) {
    uint8_t count = 0;
    if (config == NULL || cfg == NULL) {
        return -1;
    } 

    if(binding_count >= MAX_BINDINGS) {
        return -1;
    }

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"chip",(CONFIG_KEY_LENGTH -1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
    get_string(config,"chip",cfg->bindings[binding_count].configuration.parameters[count].value.string_value
        ,sizeof(cfg->bindings[binding_count].configuration.parameters[count].value.string_value));

    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"line",(CONFIG_KEY_LENGTH -1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
    get_int(config,"line",&cfg->bindings[binding_count].configuration.parameters[count].value.int_value);

    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"active_low",(CONFIG_KEY_LENGTH -1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_BOOL;
    get_bool(config,"active_low",&cfg->bindings[binding_count].configuration.parameters[count].value.bool_value);

    count++;

    cfg->bindings[binding_count].configuration.parameter_count = count;

    return 0;
}

static int32_t get_i2c(cJSON *config, uint32_t binding_count, platform_config_t *cfg) {
  
    return 0;
    uint8_t count = 0;
    if (config == NULL || cfg == NULL) {
        return -1;
    } 

    if(binding_count >= MAX_BINDINGS) {
        return -1;
    }

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"bus",(CONFIG_KEY_LENGTH -1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
    get_string(config,"bus",cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
    sizeof(cfg->bindings[binding_count].configuration.parameters[count].value.string_value));

    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"address",(CONFIG_KEY_LENGTH -1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
    get_int(config,"address",&cfg->bindings[binding_count].configuration.parameters[count].value.int_value);

    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"frequency_hz",(CONFIG_KEY_LENGTH -1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
    get_int(config,"frequency_hz",&cfg->bindings[binding_count].configuration.parameters[count].value.int_value);

    count++;

    cfg->bindings[binding_count].configuration.parameter_count = count;


    return 0;
}

void platform_print(platform_config_t* cfg) {

    if(cfg == NULL) {
        return;
    }
    // printf("Schema Version: %d\n", cfg->schema_version);
    LOG_INFO("Schema Version: %d", cfg->schema_version);
    // printf("Platform Information\n");
    LOG_INFO("Platform Information");
    // printf("===============================\n");
    LOG_INFO("===============================");

    // printf(" Name: %s\n",cfg->platform.name);
    LOG_INFO(" Name: %s",cfg->platform.name);
    // printf(" Vendor: %s\n",cfg->platform.vendor);
    LOG_INFO(" Vendor: %s",cfg->platform.vendor);
    LOG_INFO(" Soc: %s",cfg->platform.soc);
    LOG_INFO(" Description: %s",cfg->platform.description);
    LOG_INFO(" Device bind count: %d",cfg->binding_count);

    for(uint16_t index =0; index < cfg->binding_count; index++) {

        LOG_INFO("Device Info===============================\n");
        LOG_INFO("device: %s\n",cfg->bindings[index].device);
        LOG_INFO("plugin: %s\n",cfg->bindings[index].plugin);
        LOG_INFO("interface: %s\n",cfg->bindings[index].interface);
        LOG_INFO("compatible: %s\n",cfg->bindings[index].compatible);

        LOG_INFO("device parameter count %d\n",cfg->bindings[index].configuration.parameter_count);

        for(uint16_t para_count =0; para_count < cfg->bindings[index].configuration.parameter_count; para_count++) {
            LOG_INFO("Device Parameter info=======================\n");
            LOG_INFO("Device key: %s\n",cfg->bindings[index].configuration.parameters[para_count].key);
            LOG_INFO("Device type: %d\n",cfg->bindings[index].configuration.parameters[para_count].type);
            switch (cfg->bindings[index].configuration.parameters[para_count].type) {
            case CONFIG_VALUE_STRING:
                LOG_INFO("Device value: %s\n",
                    cfg->bindings[index].configuration.parameters[para_count].value.string_value);
                break;
            case CONFIG_VALUE_INT:
                LOG_INFO("Device value: %d\n",
                    cfg->bindings[index].configuration.parameters[para_count].value.int_value);
                break;
            case CONFIG_VALUE_DOUBLE:
                LOG_INFO("Device value: %lf\n",
                    cfg->bindings[index].configuration.parameters[para_count].value.double_value);
                break;

            case CONFIG_VALUE_BOOL:
                LOG_INFO("Device value: %d\n"
                    ,cfg->bindings[index].configuration.parameters[para_count].value.bool_value);
                break;
            
            default:
                break;
            }
        }
    }
}

int32_t platform_parser_load(const char* filename,platform_config_t* cfg){

   if (filename == NULL || cfg == NULL) {
        return -1;
    }

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
    cJSON *schema = cJSON_GetObjectItemCaseSensitive(root, PLATFORM_JSON_KEY_SCHEMA_VERSION);
    if (cJSON_IsNumber(schema)) {
        cfg->schema_version = schema->valueint;
    }

     // 1. Extract Platform Meta Information
    cJSON *platform = cJSON_GetObjectItemCaseSensitive(root, PLATFORM_JSON_KEY_PLATFORM);
    if (cJSON_IsObject(platform)) {
        get_string(platform, "name", cfg->platform.name, sizeof(cfg->platform.name));
    
        get_string(platform, "vendor", cfg->platform.vendor, sizeof(cfg->platform.vendor));
    
        get_string(platform, "soc", cfg->platform.soc, sizeof(cfg->platform.soc));
    
        get_string(platform, "description", cfg->platform.description, sizeof(cfg->platform.description));
    }

    cJSON *bindings = cJSON_GetObjectItemCaseSensitive(root, "bindings");
    cfg->binding_count = 0;
    if (cJSON_IsArray(bindings)) {
        cJSON *binding = NULL;
        // cJSON_ArrayForEach runs O(N) linear parsing across dynamic memory blocks
        cJSON_ArrayForEach(binding, bindings) {
            get_string(binding,"device",cfg->bindings[cfg->binding_count].device,sizeof(cfg->bindings->device));
            get_string(binding, "plugin",cfg->bindings[cfg->binding_count].plugin,sizeof(cfg->bindings->plugin));
            get_string(binding, "interface",cfg->bindings[cfg->binding_count].interface,sizeof(cfg->bindings->interface));
            get_bool(binding,"enable",&cfg->bindings[cfg->binding_count].is_enabled);
            get_string(binding, "compatible",cfg->bindings[cfg->binding_count].compatible,sizeof(cfg->bindings->compatible));
            
            cJSON *config = cJSON_GetObjectItemCaseSensitive(binding, "configuration");
            if (cJSON_IsObject(config)) {
                if(safe_string_compare(cfg->bindings->interface,"gpio")) {
                    get_gpio(config,cfg->binding_count,cfg);
                }
                else if(safe_string_compare(cfg->bindings->interface,"i2c")) {
                    get_i2c(config,cfg->binding_count,cfg);
                }
                else {
                    LOG_DEBUG("failed to match inteface %s\n",cfg->bindings->interface);
                }
            }
            cfg->binding_count++;
        }
    }
    cJSON_Delete(root);
    return 0;
}