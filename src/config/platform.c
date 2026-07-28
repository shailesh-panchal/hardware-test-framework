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

    cJSON *lines = cJSON_GetObjectItemCaseSensitive(config, "lines");
    if (cJSON_IsArray(lines)) {
        cJSON *line = NULL;
        cJSON_ArrayForEach(line, lines) {
            if (!cJSON_IsNumber(line)) {
                continue;
            }
            if (count >= MAX_CONFIG_PARAMETERS) {
                break;
            }
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"line",(CONFIG_KEY_LENGTH -1));
            cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
            cfg->bindings[binding_count].configuration.parameters[count].value.int_value = line->valueint;
            count++;
        }
    } else {
        safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"line",(CONFIG_KEY_LENGTH -1));
        cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
        get_int(config,"line",&cfg->bindings[binding_count].configuration.parameters[count].value.int_value);

        count++;
    }

    cJSON *lines_chip = cJSON_GetObjectItemCaseSensitive(config, "lines_chip");
    if (cJSON_IsArray(lines_chip)) {
        cJSON *chip = NULL;
        cJSON_ArrayForEach(chip, lines_chip) {
            if (!cJSON_IsString(chip)) {
                continue;
            }
            if (count >= MAX_CONFIG_PARAMETERS) {
                break;
            }
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"line_chip",
                (CONFIG_KEY_LENGTH -1));
            cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
                chip->valuestring,(CONFIG_KEY_LENGTH -1));
            count++;
        }
    } else if (cJSON_IsString(lines_chip)) {
        if (count < MAX_CONFIG_PARAMETERS) {
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"line_chip",
                (CONFIG_KEY_LENGTH -1));
            cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
                lines_chip->valuestring,(CONFIG_KEY_LENGTH -1));
            count++;
        }
    }

    cJSON *directions = cJSON_GetObjectItemCaseSensitive(config, "directions");
    if (cJSON_IsArray(directions)) {
        cJSON *direction = NULL;
        cJSON_ArrayForEach(direction, directions) {
            if (!cJSON_IsNumber(direction)) {
                continue;
            }
            if (count >= MAX_CONFIG_PARAMETERS) {
                break;
            }
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"direction",
                (CONFIG_KEY_LENGTH -1));
            cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
            cfg->bindings[binding_count].configuration.parameters[count].value.int_value = direction->valueint;
            count++;
        }
    } else {
        safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"direction",(CONFIG_KEY_LENGTH -1));
        cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
        get_int(config,"direction",&cfg->bindings[binding_count].configuration.parameters[count].value.int_value);
        count++;
    }

    cJSON *active_low = cJSON_GetObjectItemCaseSensitive(config, "active_low");
    if (cJSON_IsArray(active_low)) {
        cJSON *active = NULL;
        cJSON_ArrayForEach(active, active_low) {
            if (!cJSON_IsBool(active)) {
                continue;
            }
            if (count >= MAX_CONFIG_PARAMETERS) {
                break;
            }
            safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"active_low",
                (CONFIG_KEY_LENGTH -1));
            cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_BOOL;
            cfg->bindings[binding_count].configuration.parameters[count].value.bool_value = cJSON_IsTrue(active) ? 1 : 0;
            count++;
        }
    } else {
        safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key,"active_low",(CONFIG_KEY_LENGTH -1));
        cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_BOOL;
        get_bool(config,"active_low",&cfg->bindings[binding_count].configuration.parameters[count].value.bool_value);
        count++;
    }
    cfg->bindings[binding_count].configuration.parameter_count = count;

    return 0;
}

static int32_t get_i2c(cJSON *config, uint32_t binding_count, platform_config_t *cfg) {
    uint8_t count = 0;
    if (config == NULL || cfg == NULL) {
        return -1;
    }

    if (binding_count >= MAX_BINDINGS) {
        return -1;
    }

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key, "bus", (CONFIG_KEY_LENGTH - 1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
    get_string(config, "bus", cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
        sizeof(cfg->bindings[binding_count].configuration.parameters[count].value.string_value));
    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key, "address", (CONFIG_KEY_LENGTH - 1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
    get_int(config, "address", &cfg->bindings[binding_count].configuration.parameters[count].value.int_value);
    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key, "frequency_hz", (CONFIG_KEY_LENGTH - 1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_INT;
    get_int(config, "frequency_hz", &cfg->bindings[binding_count].configuration.parameters[count].value.int_value);
    count++;

    cfg->bindings[binding_count].configuration.parameter_count = count;

    return 0;
}

static int32_t get_v4l2(cJSON *config, uint32_t binding_count, platform_config_t *cfg) {
    uint8_t count = 0;
    if (config == NULL || cfg == NULL) {
        return -1;
    }

    if (binding_count >= MAX_BINDINGS) {
        return -1;
    }

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key, "device", (CONFIG_KEY_LENGTH - 1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
    get_string(config, "device", cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
        sizeof(cfg->bindings[binding_count].configuration.parameters[count].value.string_value));
    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key, "pixel_format", (CONFIG_KEY_LENGTH - 1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
    get_string(config, "pixel_format", cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
        sizeof(cfg->bindings[binding_count].configuration.parameters[count].value.string_value));
    count++;

    safe_string_copy(cfg->bindings[binding_count].configuration.parameters[count].key, "resolution", (CONFIG_KEY_LENGTH - 1));
    cfg->bindings[binding_count].configuration.parameters[count].type = CONFIG_VALUE_STRING;
    get_string(config, "resolution", cfg->bindings[binding_count].configuration.parameters[count].value.string_value,
        sizeof(cfg->bindings[binding_count].configuration.parameters[count].value.string_value));
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

        get_string(platform, "plugin_directory", cfg->platform.plugin_directory, sizeof(cfg->platform.plugin_directory));
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
                if (safe_string_compare(cfg->bindings[cfg->binding_count].interface, "gpio")) {
                    get_gpio(config, cfg->binding_count, cfg);
                }
                else if (safe_string_compare(cfg->bindings[cfg->binding_count].interface, "i2c")) {
                    get_i2c(config, cfg->binding_count, cfg);
                }
                else if (safe_string_compare(cfg->bindings[cfg->binding_count].interface, "v4l2")) {
                    get_v4l2(config, cfg->binding_count, cfg);
                }
                else {
                    LOG_DEBUG("failed to match interface %s\n", cfg->bindings[cfg->binding_count].interface);
                }
            }
            cfg->binding_count++;
        }
    }
    cJSON_Delete(root);
    return 0;
}