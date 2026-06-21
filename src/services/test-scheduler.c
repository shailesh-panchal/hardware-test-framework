#include <stdlib.h>
#include <string.h>

#include "test-scheduler.h"
#include "safe_string.h"

static int32_t test_scheduler_remove_pending(test_scheduler_t *scheduler, uint32_t index) {

    if(scheduler == NULL) {
        return -1;
    }

    if(index >= scheduler->pending_count) {
        return -2;
    }
    for(uint32_t i = index; i < scheduler->pending_count - 1; i++) {

        scheduler->pending_instances[i] = scheduler->pending_instances[i + 1];
    }

    scheduler->pending_instances[scheduler->pending_count - 1] = NULL;

    scheduler->pending_count--;
    return 0;
}

test_scheduler_t* test_scheduler_init(test_engine_t *test_engine, uint32_t max_instances){

    test_scheduler_t *scheduler = NULL;

    if(test_engine == NULL) {
        return NULL;
    }

    if(max_instances == 0){
        max_instances =  TEST_ENGINE_MAX_WORKERS;
    }
    scheduler = (test_scheduler_t *) malloc(sizeof(test_scheduler_t));
    if(scheduler == NULL) {
        return NULL;
    }

    scheduler->test_engine = test_engine;

    scheduler->max_instances = max_instances;
    scheduler->running_instances = 0;
    scheduler->state = TEST_SCHEDULER_IDLE;

    return scheduler;
}

int32_t test_scheduler_deinit(test_scheduler_t *scheduler) {

    if(scheduler == NULL) {
        return -1;
    }
    scheduler->state = TEST_SCHEDULER_IDLE;

    scheduler->running_instances = 0;
    scheduler->test_engine = NULL;

    free(scheduler);
    return 0;
}

int32_t test_scheduler_check_resource(test_scheduler_t *scheduler, test_instance_t *instance) {
    //TODO implement the logic to check the resource status 
    if(scheduler == NULL || instance == NULL)    {
        return -1;
    }
    return 0;
}

test_schedule_result_e test_scheduler_submit(test_scheduler_t *scheduler, test_instance_t *instance){

    if(scheduler == NULL || instance == NULL)    {
        return TEST_SCHEDULE_FAILED;
    }

    if(scheduler->running_instances >= scheduler->max_instances) {
        
        if(test_scheduler_check_resource(scheduler, instance) == 0) {
            scheduler->running_instances++;
            return TEST_SCHEDULE_ACCEPTED;
        }
    }

    if(scheduler->pending_count >= MAX_TEST_INSTANCES) {
        return TEST_SCHEDULE_FAILED;
    }

    scheduler->pending_instances[scheduler->pending_count] = instance;
    scheduler->pending_count++;

    return TEST_SCHEDULE_PENDING;
}

test_schedule_result_e test_scheduler_get_next_instance( test_scheduler_t *scheduler, test_instance_t **instance) {

    if(scheduler == NULL || instance == NULL) {
        return TEST_SCHEDULE_FAILED;
    }

    *instance = NULL;

    if(scheduler->running_instances >= scheduler->max_instances) {
        return TEST_SCHEDULE_PENDING;
    }

    for(uint32_t index = 0; index < scheduler->pending_count; index++) {
        test_instance_t *pending_instance;

        pending_instance = scheduler->pending_instances[index];

        if(test_scheduler_check_resource(scheduler, pending_instance) == 0) {

            test_scheduler_remove_pending(scheduler,index);

            *instance = pending_instance;
            return TEST_SCHEDULE_ACCEPTED;
        }
    }
    return TEST_SCHEDULE_PENDING;
}