#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


#include "test-engine.h"
#include "safe_string.h"
#include "util.h"

struct test_engine_t
{

    /**
     * Test Manager instance.
     *
     * Provides test metadata and availability.
     */
    test_manager_t *test_manager;


    /**
     * Registered test implementations.
     */
    test_entry_t tests[MAX_TESTS];


    /**
     * Number of registered tests.
     */
    uint32_t test_count;


    /**
     * Active test execution instances.
     */
    test_instance_t instances[MAX_TEST_INSTANCES];


    /**
     * Number of active instances.
     */
    uint32_t instance_count;
};

static void* test_engine_thread(void *arg){
    test_instance_t *instance;

    int32_t result = -1;

    if(arg == NULL) {
        return NULL;
    }
    instance = (test_instance_t *)arg;

    instance->context.start_time_ms = get_time_ms();

    if(instance->entry->operations.setup) {
        result = instance->entry->operations.setup(&instance->context);
        if(result != 0) {

            instance->result.state = TEST_STATE_FAIL;
            goto cleanup;
        }
    }

    if(instance->entry->operations.execute) {
        result = instance->entry->operations.execute(&instance->context);
        if(result == 0) {
            instance->result.state = TEST_STATE_PASS;
        }
        else {
            instance->result.state = TEST_STATE_FAIL;

        }
    }
cleanup:

    if(instance->entry->operations.cleanup){

        instance->entry->operations.cleanup(&instance->context);
    }
    instance->result.result = result;
    instance->result.execution_time_ms = get_time_ms() - instance->context.start_time_ms;

    instance->running = false;
    return NULL;
}

static test_entry_t* test_engine_find_test(test_engine_t *test_engine, const char *name) {
    if(test_engine == NULL || name == NULL){
        return NULL;
    }

    for(uint32_t index = 0; index < test_engine->test_count; index++) {

        if(safe_string_compare(test_engine->tests[index].name, name)) {

            return  &test_engine->tests[index];
        }
    }
    return NULL;
}
test_engine_t* test_engine_init(test_manager_t *test_manager,function_manager_t *function_manager,device_manager_t *device_manager) {

    test_engine_t *test_engine = NULL;
    if(test_manager == NULL || function_manager == NULL ||device_manager == NULL) {
        return NULL;
    }

    test_engine = malloc(sizeof(test_engine_t));
    if(test_engine == NULL) {
        return NULL;
    }
    test_engine->test_manager = test_manager;

    for(uint32_t index =0; index < MAX_TEST_INSTANCES; index++) {
        test_engine->instances[index].context.device_manager = device_manager;
        test_engine->instances[index].context.function_manager = function_manager;
        memset(test_engine->instances[index].context.test_name, 0, sizeof(test_engine->instances[index].context.test_name));
        test_engine->instances[index].context.timeout_ms = 0;
        test_engine->instances[index].context.start_time_ms = 0;

    }
    memset(test_engine->tests,0,sizeof(test_engine->tests));
    test_engine->test_count = 0;

    memset(test_engine->instances,0,sizeof(test_engine->instances));

    test_engine->instance_count = 0;
    return test_engine;
}

int32_t test_engine_deinit(test_engine_t *test_engine) {
    if(test_engine == NULL) {
        return -1;
    }

    for(uint32_t index = 0; index < test_engine->instance_count; index++) {

        test_instance_t *instance =
                &test_engine->instances[index];

        //TODO send the stop signal to each running test instance
        if(instance->running){
            pthread_join(instance->thread, NULL);
            instance->running = false;
        }
    }

    memset(test_engine->tests, 0, sizeof(test_engine->tests));

    test_engine->test_count = 0;
    memset(test_engine->instances, 0, sizeof(test_engine->instances));

    free(test_engine);
    return 0;
}

int32_t test_engine_register(test_engine_t *test_engine, const char *name, test_operations_t *operations) {

    test_entry_t *entry = NULL;

    if(test_engine == NULL || name == NULL || operations == NULL) {
        return -1;
    }

    if(test_engine->test_count >= MAX_TESTS) {
        return -1;
    }
    for(uint32_t index = 0; index < test_engine->test_count; index++) {
        if(safe_string_compare(test_engine->tests[index].name,name) == 0) {
            return -1;
        }
    }
    entry = &test_engine->tests[test_engine->test_count];
    safe_string_copy(entry->name,name, sizeof(entry->name));

    memcpy(&entry->operations, operations, sizeof(test_operations_t));
    test_engine->test_count++;

    return 0;
}

int32_t test_engine_execute(test_engine_t *test_engine,const char *name) {

    test_entry_t *entry = NULL;

    test_instance_t instance;

    if(test_engine == NULL || name == NULL) {
        return -1;
    }

    entry = test_engine_find_test(test_engine, name);
    if(entry == NULL){
        return -1;
    }

    if(test_engine->instance_count >= MAX_TEST_INSTANCES) {
        return -1;
    }

    memcpy(&instance, &test_engine->instances[test_engine->instance_count], sizeof(test_instance_t));
    instance.entry = entry;

    safe_string_copy(instance.context.test_name, name,sizeof(instance.context.test_name));

    instance.context.timeout_ms = entry->timeout_ms;
    safe_string_copy(instance.result.name, name,sizeof(instance.result.name));

    instance.result.state = TEST_STATE_RUNNING;
    instance.running = true;

    if(pthread_create(&instance.thread, NULL, &test_engine_thread, &instance) != 0){

        instance.running = false;
        return -1;
    }

    test_engine->instance_count++;
    return 0;
}

int32_t test_engine_execute_all(test_engine_t *test_engine) {
    if(test_engine == NULL) {
        return -1;
    }

    for(uint32_t index = 0; index < test_engine->test_count; index++){

        test_entry_t *entry;
        entry = &test_engine->tests[index];

        if(test_engine_execute(test_engine, entry->name) != 0){
            continue;
        }
    }
    return 0;
}

int32_t test_engine_get_result(test_engine_t *test_engine, const char *name, test_result_t *result){
    if(test_engine == NULL || name == NULL || result == NULL) {
        return -1;
    }

    for(uint32_t index = 0; index < test_engine->instance_count; index++){

        test_instance_t *instance;

        instance = &test_engine->instances[index];

        if(safe_string_compare(instance->result.name, name)){
            memcpy(result, &instance->result, sizeof(test_result_t));
            return 0;
        }
    }
    return -1;
}
