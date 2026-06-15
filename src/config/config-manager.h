/**
 * @file config-manager.h
 * @brief Configuration Manager interface.
 *
 * The Configuration Manager is responsible for loading and managing
 * all framework configuration files required during runtime.
 *
 * The Configuration Manager acts as the central entry point for
 * configuration handling and coordinates loading of:
 *
 * - device configuration
 * - platform configuration
 * - function configuration
 * - test configuration
 *
 * Configuration hierarchy:
 *
 * @code
 * device.json
 *      |
 *      +--> Logical device definitions
 *      |
 * platform.json
 *      |
 *      +--> Hardware bindings and platform features
 *      |
 * function.json
 *      |
 *      +--> Logical functions
 *      |
 * test.json
 *      |
 *      +--> Validation tests
 *      |
 *      v
 * Configuration Manager
 * @endcode
 *
 * Responsibilities:
 * - Load configuration files.
 * - Validate configuration consistency.
 * - Manage configuration lifecycle.
 * - Provide a unified configuration database to other modules.
 *
 * Primary consumers:
 * - Device Manager
 * - Function Manager
 * - Test Manager
 * - Plugin Manager
 * - Runtime Engine
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>
#include "device.h"
#include "platform.h"

/**
 * @brief Maximum configuration path length.
 */
#define CONFIG_MANAGER_FILE_PATH 255

/**
 * @brief Opaque Configuration Manager handle.
 *
 * Internal implementation details are hidden from users
 * of the Configuration Manager API.
 */
typedef struct config_manager_t config_manager_t;

/**
 * @brief Initialize the Configuration Manager.
 *
 * Creates and initializes a Configuration Manager instance.
 *
 * The supplied path is used as the base directory for locating
 * framework configuration files.
 *
 * Example:
 *
 * @code
 * config_manager_t *cfg =
 *     config_manager_init("./configs");
 * @endcode
 *
 * @param[in] config_path Configuration directory path.
 *
 * @return Pointer to Configuration Manager instance on success.
 * @retval NULL Initialization failure.
 */
config_manager_t* config_manager_init(
    const char *config_path);

/**
 * @brief Deinitialize the Configuration Manager.
 *
 * Releases all resources allocated by the Configuration Manager,
 * including loaded configuration data.
 *
 * @param[in] cfg_man Configuration Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_deinit(
    config_manager_t *cfg_man);

/**
 * @brief Print loaded configuration information.
 *
 * Displays the currently loaded configuration data for
 * debugging and verification purposes.
 *
 * Typical output may include:
 * - Platform information
 * - Device definitions
 * - Function definitions
 * - Test definitions
 *
 * @param[in] cfg_man Configuration Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_print(
    config_manager_t *cfg_man);

/**
 * @brief Load framework configuration files.
 *
 * Loads and validates all configuration files associated
 * with the framework.
 *
 * Typically includes:
 * - device.jsonc
 * - platform.jsonc
 * - function.jsonc
 * - test.jsonc
 *
 * Validation may include:
 * - Schema validation
 * - Device existence checks
 * - Function-to-device verification
 * - Test-to-function verification
 * - Platform binding verification
 *
 * @param[in] cfg_man Configuration Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Load or validation failure.
 */
int32_t config_manager_load(
    config_manager_t *cfg_man);

/**
 * @brief Retrieve platform information from the Configuration Manager.
 *
 * Returns the platform information loaded from the platform
 * configuration file.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[out] platform_info Pointer to a structure that receives
 *                           the platform information.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_platform_info(
    config_manager_t *cfg_man, 
    platform_info_t *platform_info);

/**
 * @brief Retrieve the number of device bindings defined for the platform.
 *
 * Returns the total number of device bindings loaded from the
 * platform configuration file.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[out] device_count Pointer to a variable that receives
 *                          the number of platform device bindings.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_platform_device_count(
    config_manager_t *cfg_man,
    uint32_t *device_count);

/**
 * @brief Retrieve a platform device binding by index.
 *
 * Returns the device binding information associated with the
 * specified index from the platform configuration.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[in] index Zero-based index of the device binding.
 * @param[out] device_binding Pointer to a structure that receives
 *                            the device binding information.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_platform_device_by_index(
    config_manager_t *cfg_man,
    uint32_t index,
    device_binding_t *device_binding);

/**
 * @brief Retrieve the number of logical devices defined in the device configuration.
 *
 * Returns the total number of device definitions loaded from the
 * device configuration file.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[out] device_count Pointer to a variable that receives
 *                          the number of configured devices.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_device_count(
    config_manager_t *cfg_man,
    uint32_t *device_count);

/**
 * @brief Retrieve a logical device definition by index.
 *
 * Returns the device definition associated with the specified
 * index from the device configuration.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[in] index Zero-based index of the device definition.
 * @param[out] device Pointer to a structure that receives
 *                    the device definition.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_device_by_index(
    config_manager_t *cfg_man,
    uint32_t index,
    device_def_t *device);

    /**
 * @brief Retrieve the number of logical function defined in the function configuration.
 *
 * Returns the total number of function definitions loaded from the
 * function configuration file.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[out] count Pointer to a variable that receives
 *                          the number of configured functions.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_function_count(
    config_manager_t *cfg_man,
    uint32_t *count);


/**
 * @brief Retrieve a logical function definition by index.
 *
 * Returns the function definition associated with the specified
 * index from the function configuration.
 *
 * @param[in] cfg_man Pointer to the Configuration Manager instance.
 * @param[in] index Zero-based index of the function definition.
 * @param[out] function Pointer to a structure that receives
 *                    the function definition.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t config_manager_get_function_by_index(
    config_manager_t *cfg_man,
    uint32_t index,
    function_def_t *function);

#endif /* CONFIG_MANAGER_H */