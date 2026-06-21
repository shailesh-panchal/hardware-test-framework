#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


#include "test-engine.h"
#include "test-scheduler.h"
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

    test_scheduler_t *scheduler;

    /**
     * Worker thread pool.
     *
     * Fixed number of execution workers.
     */
    pthread_t workers[TEST_ENGINE_MAX_WORKERS];

    /**
     * Number of active workers.
     */
    uint32_t worker_count;

    /**
     * Indicates engine running state.
     *
     * Worker threads use this flag.
     */
    bool running;

    /**
     * Test execution queue.
     *
     * Scheduler inserts runnable tests.
     * Worker threads consume them.
     */
    test_instance_t *execution_queue[TEST_ENGINE_QUEUE_SIZE];
    /**
     * Queue item count.
     */
    uint32_t queue_count;

    /**
     * Queue read index.
     */
    uint32_t queue_read_index;

    /**
     * Queue write index.
     */
    uint32_t queue_write_index;

    /**
     * Queue synchronization lock.
     */
    pthread_mutex_t queue_lock;

    /**
     * Worker notification.
     *
     * Wakes worker when new test arrives.
     */
    pthread_cond_t queue_condition;

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

static void* test_engine_run_instance(test_instance_t *instance){

    int32_t result = -1;

    if(instance == NULL) {
        return NULL;
    }
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

static int32_t test_engine_queue_push(test_engine_t *test_engine, test_instance_t *instance) {

    if(test_engine == NULL || instance == NULL) {
        return -1;
    }

    pthread_mutex_lock(&test_engine->queue_lock);

    if(test_engine->queue_count >= TEST_ENGINE_QUEUE_SIZE) {

        pthread_mutex_unlock(&test_engine->queue_lock);
        return -1;
    }

    test_engine->execution_queue[test_engine->queue_write_index] = instance;

    test_engine->queue_write_index++;
    if(test_engine->queue_write_index >= TEST_ENGINE_QUEUE_SIZE){
        test_engine->queue_write_index = 0;
    }
    test_engine->queue_count++;

    pthread_cond_signal(&test_engine->queue_condition);
    pthread_mutex_unlock(&test_engine->queue_lock);

    return 0;
}

static test_instance_t* test_engine_queue_pop( test_engine_t *test_engine) {

    test_instance_t *instance = NULL;

    if(test_engine == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&test_engine->queue_lock);

    while(test_engine->queue_count == 0 && test_engine->running) {
        pthread_cond_wait(&test_engine->queue_condition, &test_engine->queue_lock);
    }

    if(!test_engine->running) {
        pthread_mutex_unlock(&test_engine->queue_lock);
        return NULL;
    }

    instance = test_engine->execution_queue[ test_engine->queue_read_index];

    test_engine->queue_read_index++;

    if(test_engine->queue_read_index >= TEST_ENGINE_QUEUE_SIZE){
        test_engine->queue_read_index = 0;
    }

    test_engine->queue_count--;
    pthread_mutex_unlock(&test_engine->queue_lock);

    return instance;
}

static void* test_engine_worker( void *arg) {

    test_engine_t *test_engine;
    test_instance_t *instance;

    if(arg == NULL) {
        return NULL;
    }

    test_engine = (test_engine_t *)arg;

    while(test_engine->running)
    {
        instance = test_engine_queue_pop(test_engine);

        if(instance == NULL){
            continue;
        }

        test_engine_run_instance(instance);

        if(test_engine->scheduler != NULL) {
            test_schedule_result_e schedule_result = test_scheduler_get_next_instance(test_engine->scheduler,&instance);
            if(schedule_result == TEST_SCHEDULE_FAILED) {
                continue;
            }
            else if(schedule_result == TEST_SCHEDULE_ACCEPTED) {
                if(test_engine_queue_push(test_engine, instance) != 0) {
                    continue;
                }
                test_engine->instance_count++;
            }
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

    /*
     * Initialize queue state
     */
    test_engine->queue_count = 0;
    test_engine->queue_read_index = 0;
    test_engine->queue_write_index = 0;

    pthread_mutex_init(&test_engine->queue_lock, NULL);
    pthread_cond_init(&test_engine->queue_condition, NULL);

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

    test_engine->worker_count = TEST_ENGINE_MAX_WORKERS;

    //init the test scheduler 
    test_engine->scheduler  = test_scheduler_init(test_engine, TEST_ENGINE_MAX_WORKERS);
    if(test_engine->scheduler == NULL) {
        free(test_engine);
        return NULL;
    }

    for(uint32_t index = 0; index < test_engine->worker_count; index++) {

        if(pthread_create( &test_engine->workers[index], NULL, test_engine_worker,test_engine) != 0) {

            pthread_cond_broadcast( &test_engine->queue_condition);
            for(uint32_t i = 0; i < index; i++) {
                pthread_join(test_engine->workers[i], NULL);
            }
            pthread_mutex_destroy(&test_engine->queue_lock);

            pthread_cond_destroy(&test_engine->queue_condition);

            free(test_engine);
            return NULL;
        }
    } 

    test_engine->running = true;
    return test_engine;
}

int32_t test_engine_deinit(test_engine_t *test_engine) {
    if(test_engine == NULL) {
        return -1;
    }

    test_engine->running = false;

    for(uint32_t index = 0; index < test_engine->worker_count; index++) {
            pthread_join(test_engine->workers[index], NULL);
    }

    pthread_mutex_destroy(&test_engine->queue_lock);

    pthread_cond_destroy(&test_engine->queue_condition);

    memset(test_engine->tests, 0, sizeof(test_engine->tests));

    test_engine->test_count = 0;
    memset(test_engine->instances, 0, sizeof(test_engine->instances));

    if(test_engine->scheduler) {
        test_scheduler_deinit(test_engine->scheduler);
    }

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

    instance.result.state = TEST_STATE_QUEUED;
    instance.running = false;

    test_schedule_result_e schedule_result = test_scheduler_submit(test_engine->scheduler, &instance);

    if(schedule_result == TEST_SCHEDULE_FAILED) {
        return -1;
    }
    else if(schedule_result == TEST_SCHEDULE_ACCEPTED) {
       if(test_engine_queue_push(test_engine, &instance) != 0) {
            return -1;
        }
        test_engine->instance_count++;
    }
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


        if(safe_string_compare(instance->result.name, name)) {
            pthread_mutex_lock(&instance->result_lock);

            memcpy(result, &instance->result, sizeof(test_result_t));

             pthread_mutex_unlock(&instance->result_lock);
            return 0;
        }
    }
    return -1;
}
