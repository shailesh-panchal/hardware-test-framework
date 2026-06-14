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


#define PLATFORM_JSON_SCHEMA_VERSION 1
#define PLATFORM_JSON_KEY_SCHEMA_VERSION "schema_version"
#define PLATFORM_JSON_KEY_PLATFORM "platform"
#define PLATFORM_JSON_KEY_FEATURES "features"
#define PLATFORM_JSON_KEY_BINDINGS "bindings"

typedef struct {
    const char *name;
    size_t offset;
}feature_map_t;

const feature_map_t feature_registry[] = {
#define X(name) { #name, offsetof(platform_features_t, name) },
    EXPAND_ALL_FEATURES
#undef X
};

#define TOTAL_FEATURES (sizeof(feature_registry) / sizeof(feature_registry[0]))

static int32_t get_feature_category(const cJSON *obj, const char *category_name, const char *feature_name, bool *feature) {
    cJSON *category = cJSON_GetObjectItemCaseSensitive(obj, category_name);
    if (!cJSON_IsObject(category)) {
        return -1;
    }

    cJSON *feature_item = cJSON_GetObjectItemCaseSensitive(category, feature_name);
    if (cJSON_IsBool(feature_item)) {
        *feature = feature_item->valueint;
        return 0;
    }
    return -1;
}


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

static void print_feature_status(const platform_features_t *plat, const char *name) {
    for (size_t i = 0; i < TOTAL_FEATURES; i++) {
        if (safe_string_compare(feature_registry[i].name, name)) {
            const bool *status_ptr = (const bool *)((const char *)plat + feature_registry[i].offset);
            printf("%s is %s\n", feature_registry[i].name, *status_ptr ? "enabled" : "disabled");
            return;
        }
    }
    printf("Error: Feature '%s' not recognized.\n", name);
}

void platform_print(platform_config_t* cfg) {

    if(cfg == NULL) {
        return;
    }
    printf("Schema Version: %d\n", cfg->schema_version);

    printf("Platform Information\n");
    printf("===============================\n");

    printf(" Name: %s\n",cfg->platform.name);
    printf(" Vendor: %s\n",cfg->platform.vendor);
    printf(" Soc: %s\n",cfg->platform.soc);
    printf(" Description: %s\n",cfg->platform.description);
    printf(" Device bind count: %d\n",cfg->binding_count);

    
    printf("Platform supported feature list\n");
    print_feature_status(&cfg->features,"wifi");
    print_feature_status(&cfg->features,"bluetooth");
    print_feature_status(&cfg->features,"ethernet");
    print_feature_status(&cfg->features,"gsm");
    print_feature_status(&cfg->features,"lte");
    print_feature_status(&cfg->features,"fiveg");
    print_feature_status(&cfg->features,"zigbee");
    print_feature_status(&cfg->features,"nfc");
    print_feature_status(&cfg->features,"gps");
    print_feature_status(&cfg->features,"serial");
    print_feature_status(&cfg->features,"camera");
    print_feature_status(&cfg->features,"audio");
    print_feature_status(&cfg->features,"hdmi");
    print_feature_status(&cfg->features,"isp");
    print_feature_status(&cfg->features,"gpu");
    print_feature_status(&cfg->features,"npu");
    print_feature_status(&cfg->features,"internal_usb");
    print_feature_status(&cfg->features,"external_usb");
    print_feature_status(&cfg->features,"pci");
    print_feature_status(&cfg->features,"rtc");
    print_feature_status(&cfg->features,"power_monitoring");
    print_feature_status(&cfg->features,"temperature_monitoring");

    for(uint16_t index =0; index < cfg->binding_count; index++) {

        printf("Device Info===============================\n");
        printf("device: %s\n",cfg->bindings[index].device);
        printf("plugin: %s\n",cfg->bindings[index].plugin);
        printf("interface: %s\n",cfg->bindings[index].interface);
        printf("compatible: %s\n",cfg->bindings[index].compatible);

        printf("device parameter count %d\n",cfg->bindings[index].configuration.parameter_count);

        for(uint16_t para_count =0; para_count < cfg->bindings[index].configuration.parameter_count; para_count++) {
            printf("Device Parameter info=======================\n");
            printf("Device key: %s\n",cfg->bindings[index].configuration.parameters[para_count].key);
            printf("Device type: %d\n",cfg->bindings[index].configuration.parameters[para_count].type);
            switch (cfg->bindings[index].configuration.parameters[para_count].type) {
            case CONFIG_VALUE_STRING:
                printf("Device value: %s\n",
                    cfg->bindings[index].configuration.parameters[para_count].value.string_value);
                break;
            case CONFIG_VALUE_INT:
                printf("Device value: %d\n",
                    cfg->bindings[index].configuration.parameters[para_count].value.int_value);
                break;
            case CONFIG_VALUE_DOUBLE:
                printf("Device value: %lf\n",
                    cfg->bindings[index].configuration.parameters[para_count].value.double_value);
                break;

            case CONFIG_VALUE_BOOL:
                printf("Device value: %d\n"
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
            printf("Error parsing JSON before: %s\n", error_ptr);
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

    // 2. Extract Features Matrix
    cJSON *features = cJSON_GetObjectItemCaseSensitive(root, "features");
    if (cJSON_IsObject(features)) {
        get_feature_category(features, "connectivity", "wifi", &cfg->features.wifi);
        get_feature_category(features, "connectivity", "bluetooth", &cfg->features.bluetooth);
        get_feature_category(features, "connectivity", "ethernet", &cfg->features.ethernet);
        get_feature_category(features, "connectivity", "gsm", &cfg->features.gsm);
        get_feature_category(features, "connectivity", "lte", &cfg->features.lte);
        get_feature_category(features, "connectivity", "5g", &cfg->features.fiveg);
        get_feature_category(features, "connectovity", "zigbee",&cfg->features.zigbee);
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
                    printf("failed to match inteface %s\n",cfg->bindings->interface);
                }
            }
            cfg->binding_count++;
        }
    }
    cJSON_Delete(root);
    return 0;
}