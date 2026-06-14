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

#endif /* CONFIG_MANAGER_H */