/**
 * @file validation-system.h
 *
 * @brief Validation System Interface.
 *
 * Provides registration and management of
 * hardware validation implementations.
 *
 * Validation implementations are statically linked.
 *
 * Each validation module registers its operations
 * using validation_system_register_ops().
 *
 * Validation selection is controlled by configuration
 * and can be triggered from startup or CLI.
 */


#ifndef VALIDATION_SYSTEM_H
#define VALIDATION_SYSTEM_H


#include <stdint.h>

#include "test.h"
#include "runtime.h"
#include "test-operation.h"



/**
 * @brief Maximum validation modules.
 */
#define VALIDATION_MAX_COUNT MAX_TESTS

/**
 * @brief Validation registry entry.
 *
 * Maps validation name to implementation.
 */
typedef struct validation_entry_t {
    /**
     * Validation name.
     *
     * Example:
     *
     * led_indication
     */
    char name[TEST_NAME_LENGTH];

    /**
     * Validation operations.
     */
    test_operations_t *ops;
} validation_entry_t;

/**
 * @brief Validation System object.
 *
 * Owns validation registry.
 */
typedef struct validation_system_t validation_system_t;

/**
 * @brief Initialize validation system.
 *
 * Creates validation registry.
 *
 * @return Validation system instance.
 *
 * @retval NULL
 * Initialization failed.
 */
validation_system_t* validation_system_init(void);

/**
 * @brief Deinitialize validation system.
 *
 * Releases validation system resources.
 *
 * @param[in] system
 * Validation system instance.
 *
 * @return status.
 */
int32_t validation_system_deinit(
        validation_system_t *system);

/**
 * @brief Register validation implementation.
 *
 * Called by validation modules during
 * framework initialization.
 *
 * Example:
 *
 * led_indication_register()
 *
 * @param[in] system
 * Validation system instance.
 *
 * @param[in] name
 * Validation name.
 *
 * @param[in] ops
 * Validation operations.
 *
 * @return status.
 */
int32_t validation_system_register_ops(
        validation_system_t *system,
        const char *name,
        test_operations_t *ops);

/**
 * @brief Register validation into Test Engine.
 *
 * Validates:
 *
 * - Device availability
 * - Function availability
 *
 * Metadata is obtained from validation
 * configuration.
 *
 * @param[in] system
 * Validation system instance.
 *
 * @param[in] engine
 * runtime Engine instance.
 *
 * @param[in] name
 * Validation name.
 *
 * @return status.
 */
int32_t validation_system_register(
        validation_system_t *system,
        runtime_manager_t *runtime,
        const char *name);

/**
 * @brief Register all configured validations.
 *
 * Reads validation configuration and
 * registers available validations.
 *
 * @param[in] system
 * Validation system instance.
 *
 * @param[in] engine
 * runtime Engine instance.
 *
 * @return status.
 */
int32_t validation_system_register_all(
        validation_system_t *system,
        runtime_manager_t *runtime);

#endif /* VALIDATION_SYSTEM_H */