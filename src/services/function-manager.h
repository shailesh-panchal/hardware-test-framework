/**
 * @file function_manager.h
 * @brief Function Manager interface.
 *
 * The Function Manager is responsible for managing logical functions
 * defined by the Hardware Test Framework.
 *
 * A function represents a logical system capability such as:
 * - Status indication
 * - User input
 * - Temperature monitoring
 * - Camera capture
 * - Audio capture
 * - Network connectivity
 * - Video streaming
 *
 * Functions are defined in function.jsonc and reference one or more
 * logical devices defined in device.jsonc.
 *
 * The Function Manager validates function definitions against the
 * available devices registered by the Device Manager and determines
 * whether each function is available on the current platform.
 *
 * Relationship with other modules:
 *
 * Config Manager
 *      |
 *      v
 * Device Manager
 *      |
 *      v
 * Function Manager
 *      |
 *      v
 * Test Engine
 *
 * Responsibilities:
 * - Load function definitions from configuration
 * - Validate required device dependencies
 * - Resolve function-to-device mappings
 * - Determine function availability
 * - Provide function lookup services
 *
 * The Function Manager does not access hardware directly.
 * Hardware access is handled by the Device Manager, Plugin Manager,
 * and HAL layer.
 */

#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "function.h"
#include "dm.h"
#include "config-manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Runtime representation of a logical function.
 *
 * Combines the function definition loaded from configuration
 * with runtime information maintained by the Function Manager.
 */
typedef struct
{
    /**
     * Function name.
     */
    char name[FUNCTION_NAME_LENGTH];

    /**
     * Indicates whether the function is available on the
     * current platform.
     *
     * A function is considered available when all required
     * devices exist and are supported by the platform.
     */
    bool available;

    /**
     * Function definition loaded from function.jsonc.
     */
    function_def_t definition;

} Function_t;

/**
 * @brief Opaque Function Manager object.
 */
typedef struct function_manager_t function_manager_t;

/**
 * @brief Initialize the Function Manager.
 *
 * Loads all function definitions from the Configuration Manager,
 * validates device dependencies using the Device Manager,
 * and builds the runtime function database.
 *
 * During initialization, the availability of each function
 * is calculated and stored for efficient runtime lookup.
 *
 * @param[in] config_manager Pointer to the Configuration Manager.
 * @param[in] device_manager Pointer to the Device Manager.
 *
 * @return Pointer to the initialized Function Manager instance.
 * @retval NULL Initialization failed.
 */
function_manager_t* function_manager_init(
    config_manager_t* config_manager,
    device_manager_t* device_manager);

/**
 * @brief Deinitialize the Function Manager.
 *
 * Releases all resources associated with the Function Manager.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t function_manager_deinit(
    function_manager_t* fm);

/**
 * @brief Retrieve the number of registered functions.
 *
 * Returns the total number of functions managed by the
 * Function Manager.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 *
 * @return Number of registered functions.
 */
int32_t function_manager_get_count(
    function_manager_t* fm,
    uint32_t *count);

/**
 * @brief Retrieve a function by name.
 *
 * Returns the runtime function information associated with
 * the specified function name.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 * @param[in] name Function name.
 * @param[out] function Pointer to a structure that receives
 *                      the function information.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t function_manager_get_function(
    function_manager_t* fm,
    const char* name,
    function_def_t* function);

/**
 * @brief Retrieve a function by index.
 *
 * Returns the runtime function information associated with
 * the specified index.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 * @param[in] index Zero-based function index.
 * @param[out] function Pointer to a structure that receives
 *                      the function information.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t function_manager_get_function_by_index(
    function_manager_t* fm,
    uint32_t index,
    function_def_t* function);

/**
 * @brief Check whether a function is available.
 *
 * Returns the availability status of the specified function.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 * @param[in] name Function name.
 *
 * @retval true Function is available.
 * @retval false Function is unavailable or not found.
 */
bool function_manager_is_available(
    function_manager_t* fm,
    const char* name);

/**
 * @brief Retrieve the number of devices associated with a function.
 *
 * Returns the number of logical devices required by the
 * specified function.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 * @param[in] function_name Function name.
 * @param[out] count Pointer to a variable that receives
 *                   the device count.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t function_manager_get_device_count(
    function_manager_t* fm,
    const char* name,
    uint32_t* count);


/**
 * @brief Retrieve the number of devices associated with a function.
 *
 * Returns the number of logical devices required by the
 * specified function.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 * @param[in] function_name Function name.
 * @param[in] index of device in function defination.
 * @param[out] device_name Pointer to a variable that receives
 *                   the device name.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t function_manager_get_device_by_index(
    function_manager_t* fm,
    char *name,
    uint32_t device_index,
    device_name_t* device_name);

/**
 * @brief print function manager information
 *
 * Returns the number of logical devices required by the
 * specified function.
 *
 * @param[in] fm Pointer to the Function Manager instance.
 * 
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t function_manager_print(
    function_manager_t* fm);

#ifdef __cplusplus
}
#endif

#endif /* FUNCTION_MANAGER_H */