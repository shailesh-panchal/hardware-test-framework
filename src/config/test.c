#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "cJSON.h"

#include "test.h"
#include "util.h"
#include "safe_string.h"


#define TEST_JSON_SCHEMA_VERSION 1
#define TEST_JSON_KEY_SCHEMA_VERSION "schema_version"
#define TEST_JSON_KEY_TESTS "tests"
#define TEST_JSON_KEY_NAME "name"
#define TEST_JSON_KEY_DESC "description"
#define TEST_JSON_KEY_FUNCTION "function"

int32_t test_parser_load(const char* filename,test_config_t* cfg){
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
            printf("Error parsing JSON before: %s\n", error_ptr);
        }
        return -1;
    }

    // 3. Extract schema version
    cJSON *schema = cJSON_GetObjectItemCaseSensitive(root, TEST_JSON_KEY_SCHEMA_VERSION);
    if (cJSON_IsNumber(schema)) {
        cfg->schema_version = schema->valueint;
    }

    cfg->count = 0; // Initialize test count
    // 4. Step into the "tests" array
    cJSON *tests = cJSON_GetObjectItemCaseSensitive(root, TEST_JSON_KEY_TESTS);
    if (cJSON_IsArray(tests)) {
        cJSON *test = NULL;

        // 4. Iterate through every object inside the array
        cJSON_ArrayForEach(test, tests) {
            cJSON *name = cJSON_GetObjectItemCaseSensitive(test, TEST_JSON_KEY_NAME);
            if (cJSON_IsString(name) && name->valuestring != NULL) {
                original_len = safe_string_copy(cfg->tests[cfg->count].name,
                     name->valuestring, sizeof(cfg->tests[cfg->count].name));
                if (original_len >= sizeof(cfg->tests[cfg->count].name)) {
                    printf("Warning: String was truncated! Original length was %zu bytes.\n", original_len);
                }
            }

            cJSON *desc = cJSON_GetObjectItemCaseSensitive(test, TEST_JSON_KEY_DESC);
            if (cJSON_IsString(desc) && desc->valuestring != NULL) {
                original_len = safe_string_copy(cfg->tests[cfg->count].description,
                     desc->valuestring, sizeof(cfg->tests[cfg->count].description));
                if (original_len >= sizeof(cfg->tests[cfg->count].description)) {
                    printf("Warning: String was truncated! Original length was %zu bytes.\n", original_len);
                }
            }

            cJSON *func = cJSON_GetObjectItemCaseSensitive(test, TEST_JSON_KEY_FUNCTION);
            if (cJSON_IsString(func) && func->valuestring != NULL) {
                original_len = safe_string_copy(cfg->tests[cfg->count].function,
                     func->valuestring, sizeof(cfg->tests[cfg->count].function));
                if (original_len >= sizeof(cfg->tests[cfg->count].function)) {
                    printf("Warning: String was truncated! Original length was %zu bytes.\n", original_len);
                }
            }
            cfg->count++; // Increment the count of successfully parsed tests
        }
    }
    cJSON_Delete(root);
    return 0;
}

void test_printf(test_config_t *cfg) {

    if(cfg == NULL)
        return;
    
    printf("Schema Version: %d\n", cfg->schema_version);
    printf("Total Tests Parsed: %d\n", cfg->count);
    printf("===============================\n");
    for (uint32_t i = 0; i < cfg->count; i++) {
        printf("Test %d:\n", i + 1);
        printf("  Name: %s\n", cfg->tests[i].name);
        printf("  Description: %s\n", cfg->tests[i].description);
        printf("  Function: %s\n", cfg->tests[i].function);
        printf("-------------------------------\n");
    }
}
