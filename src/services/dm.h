/**
 * @file dm.h
 * @brief Device Manager interface.
 *
 * The Device Manager is responsible for maintaining the runtime device
 * registry used by the Hardware Test Framework.
 *
 * It combines:
 * - Logical device definitions from device.json
 * - Platform-specific device bindings from platform.json
 *
 * to create a unified runtime representation of each device.
 *
 * The Device Manager serves as the central repository for all devices
 * available on the target platform and provides APIs for:
 * - Device registration
 * - Device state management
 * - Device lookup
 * - Device enable/disable operations
 *
 * Relationship with other modules:
 *
 * Config Manager
 *      |
 *      v
 * Device Manager
 *      |
 *      +--> Function Manager
 *      |
 *      +--> Plugin Manager
 *      |
 *      +--> Test Engine
 *
 * The Device Manager is initialized during framework startup and
 * remains active throughout the framework lifecycle.
 */
#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#include "config-manager.h"

/**
 * @brief Runtime state of a device managed by the Device Manager.
 */
typedef enum
{
    /** Device state is unknown. */
    DEVICE_STATE_UNKNOWN = 0,

    /** Device has been registered with the Device Manager. */
    DEVICE_STATE_REGISTERED,

    /** Device is enabled and available for use. */
    DEVICE_STATE_ENABLED,

    /** Device is disabled and unavailable for use. */
    DEVICE_STATE_DISABLED,

    /** Device encountered an error condition. */
    DEVICE_STATE_ERROR

} DeviceState_e;

/**
 * @brief Runtime representation of a device.
 *
 * Combines the logical device definition from device.json
 * with the platform-specific binding information from
 * platform.json and maintains the current runtime state.
 *
 * This structure serves as the Device Manager's internal
 * device database entry.
 */
typedef struct
{
    /**
     * Unique device name.
     */
    char name[DEVICE_NAME_LENGTH];

    /**
     * Current runtime state of the device.
     */
    DeviceState_e state;

    /**
     * Logical device definition loaded from device configuration.
     */
    device_def_t definition;

    /**
     * Platform-specific device binding loaded from platform
     * configuration.
     */
    device_binding_t binding;

} Device_t;

typedef struct device_manager_t device_manager_t;

/**
 * @brief Initialize the Device Manager.
 *
 * Creates and initializes the Device Manager using the
 * configuration data provided by the Configuration Manager.
 *
 * During initialization, the Device Manager loads all logical
 * device definitions and platform device bindings into its
 * internal device registry.
 *
 * @param[in] config_manager Pointer to the Configuration Manager instance.
 *
 * @return Pointer to the initialized Device Manager instance.
 * @retval NULL Initialization failure.
 */
device_manager_t* device_manager_init(
    config_manager_t* config_manager);

/**
 * @brief Deinitialize the Device Manager.
 *
 * Releases all resources associated with the Device Manager.
 *
 * @param[in] dm Pointer to the Device Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t device_manager_deinit(
    device_manager_t* dm);

/**
 * @brief Get the number of registered devices.
 *
 * Returns the total number of devices currently managed by
 * the Device Manager.
 *
 * @param[in] dm Pointer to the Device Manager instance.
 * @param[out] count Pointer to a variable that receives
 *                          the number of configured platform device.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t device_manager_get_count(
    device_manager_t* dm,
     uint32_t *count);

/**
 * @brief Retrieve a logical device information by index.
 *
 * Returns the logical device information associated with the specified
 * index from the device manager configuration.
 *
 * @param[in] dm Pointer to the device Manager instance.
 * @param[in] index Zero-based index of the device information.
 * @param[out] function Pointer to a structure that receives
 *                    the device information.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t device_manager_get_device_by_index(
    device_manager_t* dm,
    uint32_t index,
    Device_t *device);

/**
 * @brief Retrieve the current state of a device.
 *
 * Returns the runtime state of the specified device.
 *
 * @param[in] dm Pointer to the Device Manager instance.
 * @param[in] name Device name.
 *
 * @return Current device state.
 */
DeviceState_e device_manager_get_state(
    device_manager_t *dm,
    const char* name);

/**
 * @brief Enable a device.
 *
 * Changes the device state to enabled and prepares the
 * associated hardware resource for use.
 *
 * @param[in] dm Pointer to the Device Manager instance.
 * @param[in] name Device name.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t device_manager_enable(
    device_manager_t* dm,
    const char* name);

/**
 * @brief Disable a device.
 *
 * Changes the device state to disabled and releases any
 * associated hardware resources.
 *
 * @param[in] dm Pointer to the Device Manager instance.
 * @param[in] name Device name.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t device_manager_disable(
    device_manager_t* dm,
    const char* name);

#endif // DEVICE_MANAGER_H