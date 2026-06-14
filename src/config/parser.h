#ifndef COMMON_PARSER_H
#define COMMON_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include "safe_string.h"

#include "cJSON.h"

static inline int32_t get_bool(cJSON* obj, const char* key, bool* destination) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsBool(item)) {
        if (cJSON_IsTrue(item)) {
            *destination = 1;
        }
        else {
            *destination = 0;
        }
        return 0;
    }
    return -1;
}

static inline int32_t get_int(cJSON* obj, const char* key, uint32_t* destination) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsNumber(item)) {
        *destination = (uint32_t)item->valueint;
        return 0;
    }

    return -1;
}

static inline int32_t get_string(cJSON* obj, const char* key, char* destination, size_t dest_size) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (cJSON_IsString(item) && item->valuestring != NULL) {
        safe_string_copy(destination, item->valuestring, dest_size);
        return 0;
    }
    safe_string_copy(destination, "", dest_size);
    return -1;
}
#endif //COMMNON_PARSER_H
