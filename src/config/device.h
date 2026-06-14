/**
 * @file device.h
 * @brief Device configuration parser interface.
 *
 * This module provides data structures and APIs for loading and managing
 * logical device definitions used by the Hardware Test Framework.
 *
 * A device represents a logical hardware resource that is independent of
 * any specific board, SoC, bus, or hardware implementation.
 *
 * Device definitions are intended to be reusable across multiple platforms.
 * Hardware-specific details such as GPIO numbers, I2C addresses, device
 * nodes, and drivers are defined separately in platform.json.
 *
 * Relationship with other configuration files:
 *
 * @code
 * device.json
 *      |
 *      | Defines logical devices
 *      v
 * platform.json
 *      |
 *      | Binds devices to hardware resources
 *      v
 * Device Manager
 *
 * function.json
 *      |
 *      | Uses devices to build logical functions
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
 * Example:
 *
 * @code
 * {
 *   "id": "ambient_temperature",
 *   "type": "temperature_sensor",
 *   "name": "Ambient Temperature Sensor",
 *   "description": "Primary temperature measurement device"
 * }
 * @endcode
 *
 * Design Principles:
 * - Hardware independent
 * - SoC agnostic
 * - Platform independent
 * - Stable logical identifiers
 * - Reusable across multiple products
 */

#ifndef DEVICE_PARSER_H
#define DEVICE_PARSER_H

#include <stdint.h>

/**
 * @brief Maximum number of device definitions supported.
 */
#define MAX_DEVICES 128

/**
 * @brief Maximum device identifier length.
 */
#define DEVICE_NAME_LENGTH 64

/**
 * @brief Maximum device description length.
 */
#define DEVICE_DESC_LENGTH 128

/**
 * @brief Supported logical device types.
 *
 * Device types are generic and independent of
 * hardware implementation details.
 */
typedef enum
{
    /** LED indicator device */
    DEVICE_TYPE_LED,

    /** User input button */
    DEVICE_TYPE_BUTTON,

    /** Temperature sensing device */
    DEVICE_TYPE_TEMPERATURE_SENSOR,

    /** Power source or power monitoring device */
    DEVICE_TYPE_POWER_SORCE,

    /** Accelerometer sensor */
    DEVICE_TYPE_ACCELEROMETER,

    /** Camera device */
    DEVICE_TYPE_CAMERA,

    /** Network interface */
    DEVICE_TYPE_NW_INTERFACE,

    /** Storage device */
    DEVICE_TYPE_STORAGE,

    /** Audio device */
    DEVICE_TYPE_AUDIO,

    /** Display device */
    DEVICE_TYPE_DISPLAY,

    /** Cooling fan */
    DEVICE_TYPE_FAN,

    /** Bluetooth interface */
    DEVICE_TYPE_BLUETOOTH,

    /** Neural Processing Unit */
    DEVICE_TYPE_NPU,

    /** Debug or console interface */
    DEVICE_TYPE_CONSOLE,

    /** Unknown or unsupported type */
    DEVICE_TYPE_UNKNOWN

} DeviceType_e;

/**
 * @brief Logical device definition.
 *
 * Represents a reusable hardware-independent device definition.
 */
typedef struct
{
    /**
     * @brief Unique device identifier.
     *
     * Used throughout the framework for device lookup.
     *
     * Examples:
     * - status_led
     * - ambient_temperature
     * - main_camera
     * - eth0
     */
    char id[DEVICE_NAME_LENGTH];

    /**
     * @brief Device type.
     */
    DeviceType_e type;

    /**
     * @brief Human-readable device name.
     *
     * Examples:
     * - Status LED
     * - Main Camera
     * - Ambient Temperature Sensor
     */
    char name[DEVICE_NAME_LENGTH];

    /**
     * @brief Device description.
     */
    char description[DEVICE_DESC_LENGTH];

} device_def_t;

/**
 * @brief Runtime device configuration database.
 *
 * Stores all device definitions loaded from
 * device.yaml or device.jsonc.
 */
typedef struct
{
    /**
     * @brief Configuration schema version.
     */
    uint32_t schema_version;

    /**
     * @brief Device definition table.
     */
    device_def_t devices[MAX_DEVICES];

    /**
     * @brief Number of valid device definitions.
     */
    uint32_t count;

} device_config_t;

/**
 * @brief Load device configuration file.
 *
 * Parses the specified device configuration file and
 * populates the supplied configuration structure.
 *
 * @param[in] filename Device configuration file path.
 * @param[out] cfg Device configuration database.
 *
 * @retval 0 Success.
 * @retval -1 Parse or validation failure.
 */
int32_t device_parser_load(
            const char* filename,
            device_config_t* cfg);

/**
 * @brief Print loaded device configuration.
 *
 * Displays all device definitions for debugging
 * and verification purposes.
 *
 * @param[in] cfg Device configuration database.
 */
void device_printf(
            device_config_t *cfg);

#endif /* DEVICE_PARSER_H */