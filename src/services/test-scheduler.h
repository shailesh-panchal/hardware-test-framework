/**
 * @file test_scheduler.h
 * @brief Test Scheduler service interface.
 *
 * Test Scheduler manages test execution scheduling.
 *
 * Responsibilities:
 *
 * - Limit parallel test execution
 * - Check resource availability
 * - Control test instance start
 *
 */

#ifndef TEST_SCHEDULER_H
#define TEST_SCHEDULER_H


#include <stdint.h>
#include <stdbool.h>

#include "test-engine.h"
#include "resource-manager.h"

/**
 * @brief Test scheduler state.
 */
typedef enum {
    TEST_SCHEDULER_IDLE = 0,
    TEST_SCHEDULER_RUNNING,
    TEST_SCHEDULER_ERROR
} test_scheduler_state_e;

/**
 * @brief Scheduler submit result.
 */
typedef enum
{
    TEST_SCHEDULE_ACCEPTED = 0,
    TEST_SCHEDULE_PENDING,
    TEST_SCHEDULE_FAILED
} test_schedule_result_e;

/**
 * @brief Test scheduler runtime object.
 */
typedef struct test_scheduler_t {
    /**
     * Test Engine reference.
     *
     * Scheduler uses Test Engine
     * to create execution threads.
     */
    test_engine_t *test_engine;

    /*
     * Resource ownership manager
     */
    resource_manager_t *resource_manager;

    /**
     * Maximum number of parallel tests.
     */
    uint32_t max_instances;

    /**
     * Current running test count.
     */
    uint32_t running_instances;

      /**
     * Pending test queue.
     */
    test_instance_t *pending_instances[MAX_TEST_INSTANCES];

    /**
     * Number of waiting tests.
     */
    uint32_t pending_count;

    /**
     * Scheduler state.
     */
    test_scheduler_state_e state;

}test_scheduler_t;

/**
 * @brief Initialize test scheduler.
 *
 * Creates scheduler runtime object.
 *
 * @param[in] test_engine Test Engine instance.
 * @param[in] max_instances Maximum parallel execution count.
 *
 * @return Scheduler object.
 *
 * @retval NULL Initialization failed.
 */
test_scheduler_t* test_scheduler_init(
        test_engine_t *test_engine,
        resource_manager_t *resource_manager,
        uint32_t max_instances);

/**
 * @brief Deinitialize test scheduler.
 *
 * Releases scheduler resources.
 *
 * @param[in] scheduler Scheduler instance.
 *
 * @return 0 on success.
 */
int32_t test_scheduler_deinit(
        test_scheduler_t *scheduler);

/**
 * @brief Submit test instance for execution.
 *
 * Scheduler checks:
 *
 * - Maximum running instances
 * - Resource availability
 *
 * If resources are available,
 * test execution is started.
 *
 * @param[in] scheduler Scheduler instance.
 * @param[in] instance Test instance.
 *
 * @return 0 on success.
 */
test_schedule_result_e test_scheduler_submit(
        test_scheduler_t *scheduler,
        test_instance_t *instance);

/**
 * @brief Check resource availability.
 *
 * Verifies required devices are free.
 *
 * @param[in] scheduler Scheduler instance.
 * @param[in] instance Test instance.
 *
 * @return 0 if resources available.
 */
int32_t test_scheduler_check_resource(
        test_scheduler_t *scheduler,
        test_instance_t *instance);

/**
 * @brief Get running test count.
 *
 * @param[in] scheduler Scheduler instance.
 * @param[out] count Running instance count.
 *
 * @return 0 on success.
 */
int32_t test_scheduler_get_running_count(
        test_scheduler_t *scheduler,
        uint32_t *count);

/**
 * @brief Get next runnable test instance.
 *
 * Scheduler searches pending queue and returns
 * the next test instance which can execute.
 *
 * Checks:
 *
 * - Resource availability
 * - Execution slot availability
 *
 * @param[in] scheduler Test Scheduler instance.
 * @param[out] instance Next executable instance.
 *
 * @return 0 if instance available.
 *
 * @retval -1 Invalid parameter.
 * @retval -2 No runnable instance available.
 */
test_schedule_result_e test_scheduler_get_next_instance(
        test_scheduler_t *scheduler,
        test_instance_t **instance);
#endif /* TEST_SCHEDULER_H */