/**
 * @file test.h
 * @brief Test configuration parser interface.
 *
 * This module provides APIs and data structures for loading and managing
 * test definitions from the framework test configuration file
 * (test.yaml or test.jsonc).
 *
 * A test definition represents a validation test that is associated with
 * a logical function defined in function.yaml.
 *
 * The Test Parser is responsible for:
 * - Parsing test configuration files.
 * - Validating test definitions.
 * - Building the runtime test configuration database.
 * - Providing test metadata to the Test Manager.
 *
 * Relationship with other configuration files:
 *
 * @code
 * device.json
 *      |
 *      v
 * function.json
 *      |
 *      v
 * test.json
 * @endcode
 *
 * Example:
 *
 * @code
 * tests:
 *   - name: temperature_monitoring_test
 *     description: Verify temperature monitoring functionality
 *     function: temperature_monitoring
 * @endcode
 *
 * @author
 * Hardware Test Framework
 *
 * @copyright
 * MIT License
 */

#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <stdint.h>
#include "device.h"
#include "function.h"

/**
 * @brief Maximum number of test definitions supported.
 */
#define MAX_TESTS 128

/**
 * @brief Maximum length of a test name.
 */
#define TEST_NAME_LENGTH 64

/**
 * @brief Maximum length of a test description.
 */
#define TEST_DESC_LENGTH 128

/**
 * @brief Test definition.
 *
 * Represents a single validation test within the framework.
 *
 * A test is associated with exactly one logical function.
 * The actual test execution logic is implemented by the
 * Test Manager and corresponding test plugins.
 */
typedef struct
{
    /**
     * @brief Unique test name.
     *
     * Example:
     * - temperature_monitoring_test
     * - camera_capture_test
     */
    char name[TEST_NAME_LENGTH];

    /**
     * @brief Human-readable test description.
     */
    char description[TEST_DESC_LENGTH];

    /**
     * @brief Associated function name.
     *
     * Must match a function defined in function.yaml.
     *
     * Example:
     * - temperature_monitoring
     * - camera_capture
     */
    char function[FUNCTION_NAME_LENGTH];

} test_def_t;

/**
 * @brief Runtime test configuration database.
 *
 * Stores all parsed test definitions loaded from
 * the configuration file.
 */
typedef struct
{
    /**
     * @brief Configuration schema version.
     *
     * Used for backward compatibility and validation.
     */
    uint32_t schema_version;

    /**
     * @brief Array of test definitions.
     */
    test_def_t tests[MAX_TESTS];

    /**
     * @brief Number of valid test definitions loaded.
     */
    uint32_t count;

} test_config_t;

/**
 * @brief Load test configuration file.
 *
 * Parses the specified test configuration file and populates
 * the provided configuration structure.
 *
 * @param[in] filename Path to test configuration file.
 * @param[out] cfg Pointer to test configuration database.
 *
 * @retval 0 Success.
 * @retval -1 Failed to load or parse file.
 */
int32_t test_parser_load(
            const char* filename,
            test_config_t* cfg);

/**
 * @brief Print loaded test configuration.
 *
 * Displays all parsed test definitions for debugging
 * and verification purposes.
 *
 * @param[in] cfg Pointer to test configuration database.
 */
void test_printf(
            test_config_t *cfg);

#endif /* TEST_PARSER_H */