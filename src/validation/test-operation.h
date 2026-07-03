/**
 * @file test-operation.h
 *
 * @brief Test operation interface.
 *
 * Defines the common lifecycle interface for
 * validation/test implementations.
 *
 * Each validation provides these operations and
 * registers them with validation-system.
 *
 * Example:
 *
 * led_indication_test
 * camera_capture_test
 *
 */


#ifndef TEST_OPS_H
#define TEST_OPS_H


#include <stdint.h>

/**
 * @brief Test operation interface.
 *
 * Every validation/test implementation provides
 * this interface.
 *
 * Lifecycle:
 *
 *        setup()
 *
 *          |
 *          v
 *
 *       execute()
 *
 *          |
 *          v
 *
 *       cleanup()
 *
 */
typedef struct 
{
    /**
     * @brief Initialize test.
     *
     * Called before execution.
     *
     * Responsibilities:
     *
     * - Allocate test resources
     * - Open required plugin
     * - Prepare test state
     *
     * @param[out] context
     * Test private context.
     *
     * @return status.
     */
    int32_t (*setup)(void* context);
    /**
     * @brief Execute test.
     *
     * Main validation logic.
     *
     * Example:
     *
     * Toggle GPIO
     * Capture image
     * Validate result
     *
     * @param[in] context
     * Test context.
     *
     * @return status.
     */
    int32_t (*execute)(void* context);
    /**
     * @brief Cleanup test.
     *
     * Called after execution.
     *
     * Responsibilities:
     *
     * - Release resources
     * - Close plugin
     * - Free memory
     *
     * @param[in] context
     * Test context.
     *
     * @return status.
     */
    int32_t (*cleanup)(void* context);
}test_operations_t;






#endif /* TEST_OPS_H */