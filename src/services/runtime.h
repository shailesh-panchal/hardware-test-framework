/**
 * @file runtime.h
 * @brief Runtime Manager interface.
 *
 * The Runtime Manager is responsible for orchestrating the initialization,
 * startup, and shutdown sequence of all framework managers.
 *
 * It acts as the central entry point for the Hardware Test Framework and
 * owns the lifecycle of the following components:
 *
 * - Configuration Manager
 * - Device Manager
 * - Function Manager
 * - Test Manager
 *
 * Future versions may also include:
 *
 * - Logger
 * - Plugin Manager
 * - Capability Manager
 * - Test Engine
 * - Report Generator
 * - Telemetry Service
 *
 * Initialization Order:
 *
 * @code
 * Config Manager
 *      |
 *      v
 * Device Manager
 *      |
 *      v
 * Function Manager
 *      |
 *      v
 * Test Manager
 * @endcode
 *
 * Shutdown is performed in the reverse order.
 */

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdint.h>

#include "config-manager.h"
#include "dm.h"
#include "function-manager.h"
#include "test-manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct runtime_manager_t runtime_manager_t;

/**
 * @brief Initialize the framework runtime.
 *
 * Creates and initializes all framework managers in the required
 * dependency order.
 *
 * Initialization sequence:
 * - Configuration Manager
 * - Device Manager
 * - Function Manager
 * - Test Manager
 *
 * If any initialization step fails, all previously initialized
 * components are automatically cleaned up.
 *
 * @param[in] config_path Path to the configuration directory.
 *
 * @return Pointer to an initialized Runtime Manager instance.
 * @retval NULL Initialization failed.
 */
runtime_manager_t* runtime_manager_init(
    const char *config_path);

/**
 * @brief Shutdown the framework runtime.
 *
 * Releases all framework resources and deinitializes all managers
 * in reverse dependency order.
 *
 * Shutdown sequence:
 * - Test Manager
 * - Function Manager
 * - Device Manager
 * - Configuration Manager
 *
 * @param[in] runtime Pointer to the Runtime Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t runtime_manager_deinit(
    runtime_manager_t *runtime);

/**
 * @brief Get the Configuration Manager instance.
 *
 * @param[in] runtime Pointer to the Runtime Manager instance.
 *
 * @return Pointer to the Configuration Manager.
 */
config_manager_t* runtime_manager_get_config_manager(
    runtime_manager_t *runtime);

/**
 * @brief Get the Device Manager instance.
 *
 * @param[in] runtime Pointer to the Runtime Manager instance.
 *
 * @return Pointer to the Device Manager.
 */
device_manager_t* runtime_manager_get_device_manager(
    runtime_manager_t *runtime);

/**
 * @brief Get the Function Manager instance.
 *
 * @param[in] runtime Pointer to the Runtime Manager instance.
 *
 * @return Pointer to the Function Manager.
 */
function_manager_t* runtime_manager_get_function_manager(
    runtime_manager_t *runtime);

/**
 * @brief Get the Test Manager instance.
 *
 * @param[in] runtime Pointer to the Runtime Manager instance.
 *
 * @return Pointer to the Test Manager.
 */
test_manager_t* runtime_manager_get_test_manager(
    runtime_manager_t *runtime);

#ifdef __cplusplus
}
#endif

#endif /* RUNTIME_H */