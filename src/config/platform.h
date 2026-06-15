/**
 * @file platform.h
 * @brief Platform configuration parser interface.
 *
 * This module provides data structures and APIs for loading and managing
 * platform-specific configuration information used by the Hardware Test
 * Framework.
 *
 * The platform configuration describes:
 * - Platform identification information.
 * - Platform feature availability.
 * - Device-to-hardware bindings.
 * - Hardware-specific configuration parameters.
 *
 * The platform configuration serves as the bridge between logical devices
 * defined in device.json and the actual hardware resources available on
 * a specific board or SoC.
 *
 * Relationship with other configuration files:
 *
 * @code
 * device.json
 *      |
 *      +--> Defines logical devices
 *      |
 * platform.json
 *      |
 *      +--> Maps logical devices to hardware resources
 *      |
 * Device Manager
 *
 * function.json
 *      |
 * Function Manager
 *
 * test.json
 *      |
 * Test Manager
 * @endcode
 */
#ifndef PLATFORM_PARSER_H
#define PLATFORM_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>


#include "device.h"
#include "function.h"



#define MAX_BINDINGS 64
#define PLATFORM_NAME_LENGTH 64
#define PLATFORM_VENDOR_LENGTH 64
#define PLATFORM_SOC_LENGTH 64
#define PLATFORM_DESC_LENGTH 256
#define PLUGIN_NAME_LENGTH 64
#define INTERFACE_NAME_LENGTH 32
#define COMPATIBLE_NAME_LENGTH 64
#define MAX_CONFIG_PARAMETERS 32
#define CONFIG_KEY_LENGTH     64
#define CONFIG_STRING_LENGTH 128

/**
 * @brief Supported configuration parameter types.
 *
 * Used to represent hardware-specific binding parameters
 * in a generic key-value format.
 */
typedef enum
{
    CONFIG_VALUE_STRING = 0, /**< String value */
    CONFIG_VALUE_INT,        /**< Integer value */
    CONFIG_VALUE_DOUBLE,     /**< Floating point value */
    CONFIG_VALUE_BOOL        /**< Boolean value */

} config_value_type_t;

/**
 * @brief Generic configuration parameter.
 *
 * Represents a hardware-specific configuration item
 * associated with a device binding.
 *
 * Example:
 * @code
 * bus = "/dev/i2c-1"
 * address = 0x48
 * active_low = true
 * @endcode
 */
typedef struct
{
    /** Parameter name */
    char key[CONFIG_KEY_LENGTH];

    /** Parameter value type */
    config_value_type_t type;

    /** Parameter value */
    union
    {
        char string_value[CONFIG_STRING_LENGTH];
        uint32_t int_value;
        double double_value;
        bool bool_value;
    } value;

} config_parameter_t;

/**
 * @brief Collection of configuration parameters.
 *
 * Stores all hardware-specific configuration parameters
 * associated with a device binding.
 */
typedef struct
{
    /** Number of valid parameters */
    uint32_t parameter_count;

    /** Configuration parameter list */
    config_parameter_t parameters[MAX_CONFIG_PARAMETERS];

} configuration_t;


/**
 * @brief Device binding definition.
 *
 * Maps a logical device defined in device.json to a
 * hardware-specific implementation on the platform.
 */
typedef struct
{
    /**
     * @brief Logical device name.
     *
     * Must match a device defined in device.json.
     */
    char device[DEVICE_NAME_LENGTH];

    /**
     * @brief Plugin implementation name.
     *
     * Examples:
     * - gpio_linux
     * - i2c_linux
     * - spi_linux
     * - v4l2_camera
     */
    char plugin[PLUGIN_NAME_LENGTH];

    /**
     * @brief Hardware interface type.
     *
     * Examples:
     * - gpio
     * - i2c
     * - spi
     * - uart
     * - video
     */
    char interface[INTERFACE_NAME_LENGTH];

    /**
     * @brief Compatible device identifier.
     *
     * Similar to Linux Device Tree compatible strings.
     */
    char compatible[COMPATIBLE_NAME_LENGTH];

    /**
     * @brief Hardware-specific configuration.
     */
    configuration_t configuration;

} device_binding_t;

/**
 * @brief Platform identification information.
 */
typedef struct
{
    /** Platform name */
    char name[PLATFORM_NAME_LENGTH];

    /** Platform vendor */
    char vendor[PLATFORM_VENDOR_LENGTH];

    /** SoC name */
    char soc[PLATFORM_SOC_LENGTH];

    /** Human-readable platform description */
    char description[PLATFORM_DESC_LENGTH];

} platform_info_t;


// ============================================================================
// 1. THE SINGLE SOURCE OF TRUTH
// Add, remove, or reorder features here. The rest of the code updates itself!
// ============================================================================
#define EXPAND_ALL_FEATURES \
    X(wifi) \
    X(bluetooth) \
    X(ethernet) \
    X(gsm) \
    X(lte) \
    X(fiveg) \
    X(zigbee) \
    X(nfc) \
    X(gps) \
    X(serial) \
    X(camera) \
    X(audio) \
    X(hdmi) \
    X(isp) \
    X(gpu) \
    X(npu) \
    X(internal_usb) \
    X(external_usb) \
    X(pci) \
    X(rtc) \
    X(power_monitoring) \
    X(temperature_monitoring)

/**
 * @brief Platform feature set.
 *
 * Represents hardware capabilities available on
 * a particular platform.
 *
 * These features are primarily used by the Function Manager
 * and Test Manager to determine availability of functions
 * and tests.
 */
typedef struct {
#define X(name) bool name;
    EXPAND_ALL_FEATURES
#undef X
} platform_features_t;

/**
 * @brief Runtime platform configuration database.
 *
 * Stores all information loaded from platform.json.
 */
typedef struct
{
    /**
     * @brief Configuration schema version.
     */
    uint32_t schema_version;

    /**
     * @brief Platform information.
     */
    platform_info_t platform;

    /**
     * @brief Platform feature set.
     */
    platform_features_t features;

    /**
     * @brief Number of valid device bindings.
     */
    uint32_t binding_count;

    /**
     * @brief Device binding table.
     */
    device_binding_t bindings[MAX_BINDINGS];

} platform_config_t;

/**
 * @brief Load platform configuration file.
 *
 * Parses a platform configuration file and populates
 * the supplied platform configuration structure.
 *
 * @param[in] filename Platform configuration file path.
 * @param[out] cfg Platform configuration structure.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t platform_parser_load(const char* filename,platform_config_t* cfg);

/**
 * @brief Print platform configuration.
 *
 * Displays platform information, features, and bindings
 * for debugging and verification purposes.
 *
 * @param[in] cfg Platform configuration structure.
 */
void platform_print(platform_config_t* cfg);

#endif // PLATFORM_PARSER_H