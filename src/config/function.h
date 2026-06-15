/**
 * @file function.h
 * @brief Function configuration parser interface.
 *
 * This module provides data structures and APIs for loading and managing
 * logical function definitions used by the Hardware Test Framework.
 *
 * A function represents a logical capability provided by one or more
 * devices. Functions abstract hardware implementation details and provide
 * a stable interface for test execution.
 *
 * Relationship with other configuration files:
 *
 * @code
 * device.json
 *      |
 *      | Defines logical devices
 *      v
 * function.json
 *      |
 *      | Groups one or more devices into a logical function
 *      v
 * Function Manager
 *
 * test.json
 *      |
 *      | References functions
 *      v
 * Test Manager
 * @endcode
 *
 * Examples:
 *
 * Single-device function:
 * @code
 * {
 *   "name": "temperature_monitoring",
 *   "description": "Monitor system temperature",
 *   "devices": ["ambient_temperature"]
 * }
 * @endcode
 *
 * Multi-device function:
 * @code
 * {
 *   "name": "audio_video_recording",
 *   "description": "Record audio and video",
 *   "devices": [
 *      "main_camera",
 *      "microphone"
 *   ]
 * }
 * @endcode
 *
 * Design Principles:
 * - Hardware independent
 * - Platform independent
 * - Device abstraction layer
 * - Test-facing interface
 */

#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include <stdint.h>
#include "device.h"

/**
 * @brief Maximum number of function definitions supported.
 */
#define MAX_FUNCTIONS 128

/**
 * @brief Maximum function name length.
 */
#define FUNCTION_NAME_LENGTH 64

/**
 * @brief Maximum function description length.
 */
#define FUNCTION_DESC_LENGTH 128

/**
 * @brief Device name reference.
 *
 * Represents a logical device associated with a function.
 * The referenced device must exist in device.json.
 */
typedef struct
{
    /**
     * @brief Device name.
     *
     * Must match a device defined in device.json.
     */
    char name[FUNCTION_NAME_LENGTH];

} device_name_t;

/**
 * @brief Function definition.
 *
 * Represents a logical capability exposed to the test framework.
 *
 * A function may be associated with:
 * - A single device
 * - Multiple devices
 *
 * Examples:
 * - status_indication
 * - temperature_monitoring
 * - network_connectivity
 * - audio_video_recording
 */
typedef struct
{
    /**
     * @brief Unique function name.
     */
    char name[FUNCTION_NAME_LENGTH];

    /**
     * @brief Human-readable function description.
     */
    char description[FUNCTION_DESC_LENGTH];

    /**
     * @brief List of devices used by this function.
     */
    device_name_t device_name[MAX_DEVICES];

    /**
     * @brief Number of associated devices.
     */
    uint32_t count;

} function_def_t;

/**
 * @brief Runtime function configuration database.
 *
 * Stores all function definitions loaded from function.json
 * or function.jsonc.
 */
typedef struct
{
    /**
     * @brief Configuration schema version.
     */
    uint32_t schema_version;

    /**
     * @brief Function definition table.
     */
    function_def_t functions[MAX_FUNCTIONS];

    /**
     * @brief Number of valid function definitions.
     */
    uint32_t count;

} function_config_t;

/**
 * @brief Load function configuration file.
 *
 * Parses the specified function configuration file and
 * populates the supplied configuration structure.
 *
 * @param[in] filename Configuration file path.
 * @param[out] cfg Function configuration database.
 *
 * @retval 0 Success.
 * @retval -1 Parse or validation failure.
 */
int32_t function_parser_load(
            const char* filename,
            function_config_t* cfg);

/**
 * @brief Print function configuration.
 *
 * Displays all loaded functions and their associated
 * devices for debugging and verification purposes.
 *
 * @param[in] cfg Function configuration database.
 */
void function_print(
            function_config_t* cfg);

#endif // FUNCTION_PARSER_H