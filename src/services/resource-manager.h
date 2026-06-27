/**
 * @file resource-manager.h
 *
 * @brief Resource Manager interface.
 *
 * The Resource Manager is responsible for managing
 * shared resource ownership between parallel test
 * instances.
 *
 * Responsibilities:
 *
 * - Register available resources
 * - Track resource ownership
 * - Check resource availability
 * - Acquire resources for test execution
 * - Release resources after test completion
 *
 * Resource Manager does not control hardware.
 * Hardware access is handled by Device Manager
 * and HAL layers.
 *
 */


#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H


#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "device.h"
#include "test-engine.h"
#include "function-manager.h"


/**
 * @brief Maximum resources supported.
 */
#define RESOURCE_MANAGER_MAX_RESOURCES MAX_DEVICES


/**
 * @brief Resource state.
 */
typedef enum
{

    /**
     * Resource is available.
     */
    RESOURCE_STATE_FREE = 0,

    /**
     * Resource is currently owned by
     * a test instance.
     */
    RESOURCE_STATE_BUSY

} resource_state_e;

/**
 * @brief Resource information.
 *
 * Stores resource ownership and state.
 */
typedef struct resource_info_t
{

    /**
     * Resource name.
     *
     * Example:
     *
     * camera0
     * i2c0
     * gpio17
     */
    char name[DEVICE_NAME_LENGTH];

    /**
     * Current resource state.
     */
    resource_state_e state;

    /**
     * Test instance owning this resource.
     *
     * NULL when resource is free.
     */
    test_instance_t *owner;

} resource_info_t;

/**
 * @brief Resource Manager object.
 *
 * Maintains all registered resources and
 * protects resource ownership information.
 */
typedef struct resource_manager_t resource_manager_t;

/**
 * @brief Initialize Resource Manager.
 *
 * @param[in] function_manager
 *        Function Manager instance.
 *
 * @return Resource Manager instance.
 */
resource_manager_t* resource_manager_init(
        function_manager_t *function_manager);

/**
 * @brief Deinitialize Resource Manager.
 *
 * Releases allocated resources.
 *
 * @param[in] manager
 *        Resource Manager instance.
 *
 * @return Execution status.
 *
 * @retval 0
 *         Success.
 *
 * @retval -1
 *         Invalid parameter.
 */
int32_t resource_manager_deinit(
        resource_manager_t *manager);

/**
 * @brief Register resource.
 *
 * Adds a new resource into the manager.
 *
 * Example:
 *
 * camera0
 * i2c0
 * v4l2_stream0
 *
 * @param[in] manager
 *        Resource Manager instance.
 *
 * @param[in] name
 *        Resource name.
 *
 * @return Execution status.
 *
 * @retval 0
 *         Success.
 *
 * @retval -1
 *         Invalid parameter.
 *
 * @retval -2
 *         Resource limit reached.
 */
int32_t resource_manager_register(
        resource_manager_t *manager,
        const char *name);

/**
 * @brief Check function resource availability.
 *
 * Queries Function Manager for required
 * resources and checks runtime state.
 *
 * @param[in] manager
 *        Resource Manager instance.
 *
 * @param[in] function_name
 *        Function name.
 *
 * @return true if available.
 */
bool resource_manager_is_available(
        resource_manager_t *manager,
        const char *function_name);

/**
 * @brief Acquire resources for function.
 *
 * Marks all resources required by the
 * function as busy.
 *
 * @param[in] manager
 *        Resource Manager instance.
 *
 * @param[in] function_name
 *        Function name.
 *
 * @param[in] owner
 *        Test instance name.
 */
int32_t resource_manager_acquire(
        resource_manager_t *manager,
        const char *function_name,
        test_instance_t *instance);

/**
 * @brief Release resources.
 *
 * Marks resources as free.
 */
int32_t resource_manager_release(
        resource_manager_t *manager,
        const char *function_name,
        test_instance_t *instance);

#endif /* RESOURCE_MANAGER_H */