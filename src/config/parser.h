/**
 * @file parser.h
 * @brief Common helper functions for configuration parsing.
 *
 * This module provides utility functions used by various configuration
 * parsers (device, function, platform, and test parsers) to safely
 * extract values from cJSON objects.
 *
 * Supported data types:
 * - Boolean
 * - Integer
 * - String
 *
 * These helper functions perform:
 * - Type validation
 * - Null checking
 * - Safe data extraction
 * - Safe string copying
 *
 * The functions return:
 * - 0 on success
 * - -1 on failure
 *
 * Example:
 *
 * @code
 * uint32_t value;
 *
 * if (get_int(json_obj, "timeout", &value) == 0)
 * {
 *     printf("Timeout = %u\n", value);
 * }
 * @endcode
 */

#ifndef COMMON_PARSER_H
#define COMMON_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include "safe_string.h"

#include "cJSON.h"
/**
 * @brief Extract a boolean value from a JSON object.
 *
 * Retrieves the specified key from a cJSON object and stores
 * the boolean value into the provided destination.
 *
 * @param[in] obj Source cJSON object.
 * @param[in] key JSON key name.
 * @param[out] destination Destination boolean variable.
 *
 * @retval 0 Success.
 * @retval -1 Key not found or value is not a boolean.
 */
static inline int32_t get_bool(
    cJSON* obj,
    const char* key,
    bool* destination)
{
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

/**
 * @brief Extract an integer value from a JSON object.
 *
 * Retrieves the specified key from a cJSON object and stores
 * the value into the provided destination.
 *
 * @param[in] obj Source cJSON object.
 * @param[in] key JSON key name.
 * @param[out] destination Destination integer variable.
 *
 * @retval 0 Success.
 * @retval -1 Key not found or value is not numeric.
 */
static inline int32_t get_int(
    cJSON* obj,
    const char* key,
    uint32_t* destination)
{
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsNumber(item)) {
        *destination = (uint32_t)item->valueint;
        return 0;
    }

    return -1;
}

/**
 * @brief Extract a string value from a JSON object.
 *
 * Retrieves the specified key from a cJSON object and safely
 * copies the string value into the destination buffer.
 *
 * If the key is missing or the value is not a string,
 * an empty string is copied into the destination buffer.
 *
 * @param[in] obj Source cJSON object.
 * @param[in] key JSON key name.
 * @param[out] destination Destination buffer.
 * @param[in] dest_size Destination buffer size.
 *
 * @retval 0 Success.
 * @retval -1 Key not found or value is not a string.
 */
static inline int32_t get_string(
    cJSON* obj,
    const char* key,
    char* destination,
    size_t dest_size)
{
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsString(item) && item->valuestring != NULL) {
        safe_string_copy(destination, item->valuestring, dest_size);
        return 0;
    }

    safe_string_copy(destination, "", dest_size);
    return -1;
}

#endif /* COMMON_PARSER_H */