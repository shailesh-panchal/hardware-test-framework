/**
 * @file test_manager.h
 * @brief Test Manager interface.
 *
 * The Test Manager is responsible for managing validation tests
 * defined in the framework configuration.
 *
 * A test represents a validation scenario associated with a
 * logical function. Test definitions are loaded from test.jsonc
 * and validated against functions registered by the Function Manager.
 *
 * The Test Manager determines whether a test is available on the
 * current platform by verifying the availability of its associated
 * function.
 *
 * Relationship with other modules:
 *
 * Config Manager
 *      |
 *      v
 * Function Manager
 *      |
 *      v
 * Test Manager
 *      |
 *      v
 * Test Engine
 *
 * Responsibilities:
 * - Load test definitions from configuration
 * - Validate function dependencies
 * - Determine test availability
 * - Provide test lookup services
 * - Resolve test-to-function mappings
 *
 * The Test Manager does not execute tests. Test execution is
 * handled by the Test Engine.
 */

#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "test.h"
#include "function-manager.h"
#include "config-manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Runtime state of a test.
 */
typedef enum
{
    /**
     * Test state is unknown.
     */
    TEST_STATE_UNKNOWN = 0,

    /**
     * Test is available for execution.
     */
    TEST_STATE_AVAILABLE,

    /**
     * Test is unavailable because one or more dependencies
     * are not available.
     */
    TEST_STATE_UNAVAILABLE,

    /**
     * Test configuration contains errors.
     */
    TEST_STATE_ERROR

} TestState_e;

/**
 * @brief Runtime representation of a test.
 *
 * Contains the test definition loaded from configuration
 * along with runtime status information maintained by the
 * Test Manager.
 */
typedef struct
{
    /**
     * Test name.
     */
    char name[TEST_NAME_LENGTH];

    /**
     * Current runtime state of the test.
     */
    TestState_e state;

    /**
     * Test definition loaded from test.jsonc.
     */
    test_def_t definition;

} Test_t;

/**
 * @brief Test Manager object.
 *
 * Stores all registered tests and their runtime status.
 */
typedef struct test_manager_t test_manager_t;

/**
 * @brief Initialize the Test Manager.
 *
 * Loads all test definitions from the Configuration Manager,
 * validates referenced functions using the Function Manager,
 * and builds the runtime test database.
 *
 * Test availability is determined during initialization.
 *
 * @param[in] config_manager Pointer to the Configuration Manager.
 * @param[in] function_manager Pointer to the Function Manager.
 *
 * @return Pointer to an initialized Test Manager instance.
 * @retval NULL Initialization failed.
 */
test_manager_t* test_manager_init(
    config_manager_t* config_manager,
    function_manager_t* function_manager);

/**
 * @brief Deinitialize the Test Manager.
 *
 * Releases all resources associated with the Test Manager.
 *
 * @param[in] tm Pointer to the Test Manager instance.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t test_manager_deinit(
    test_manager_t* tm);

/**
 * @brief Get the total number of registered tests.
 *
 * @param[in] tm Pointer to the Test Manager instance.
 * @param[out] total count of test supported by test manager
 
 * @return Number of registered tests.
 */
int32_t test_manager_get_count(
    test_manager_t* tm,
    uint32_t *count);

/**
 * @brief Get test information by name.
 *
 * Retrieves the runtime test information associated with
 * the specified test name.
 *
 * @param[in] tm Pointer to the Test Manager instance.
 * @param[in] name Test name.
 * @param[out] test Pointer to a structure that receives
 *                  the test information.
 *
 * @retval 0 Success.
 * @retval -1 Test not found or invalid parameters.
 */
int32_t test_manager_get_test(
    test_manager_t* tm,
    const char* name,
    test_def_t* test);

/**
 * @brief Get test information by index.
 *
 * Retrieves the runtime test information associated with
 * the specified index.
 *
 * @param[in] tm Pointer to the Test Manager instance.
 * @param[in] index Zero-based test index.
 * @param[out] test Pointer to a structure that receives
 *                  the test information.
 *
 * @retval 0 Success.
 * @retval -1 Invalid index or parameters.
 */
int32_t test_manager_get_test_by_index(
    test_manager_t* tm,
    uint32_t index,
    test_def_t* test);

/**
 * @brief Check whether a test is available.
 *
 * A test is considered available when its associated function
 * is available on the current platform.
 *
 * @param[in] tm Pointer to the Test Manager instance.
 * @param[in] name Test name.
 *
 * @retval true Test is available.
 * @retval false Test is unavailable or not found.
 */
bool test_manager_test_is_available(
    test_manager_t* tm,
    const char* name);

/**
 * @brief Get the function name associated with a test.
 *
 * Retrieves the logical function referenced by the specified
 * test definition.
 *
 * @param[in] tm Pointer to the Test Manager instance.
 * @param[in] test_name Test name.
 * @param[out] function_name Buffer to receive the function name.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int32_t test_manager_get_function_name(
    test_manager_t* tm,
    const char* name,
    char* function_name);

/**
 * @brief Print test managerinformation associated with a test.
 *
 */
void test_manager_printf(
    test_manager_t *tm);

#ifdef __cplusplus
}
#endif

#endif /* TEST_MANAGER_H */