/**
 * @file validation-system.c
 *
 * @brief Validation System implementation.
 */


#include <stdlib.h>
#include <string.h>


#include "validation-system.h"
#include "validation-registry.h"
#include "runtime.h"
#include "safe_string.h"

/**
 * @brief Validation System object.
 *
 * Owns validation registry.
 */
struct validation_system_t {
    /**
     * Registered validations.
     */
    validation_entry_t validations[VALIDATION_MAX_COUNT];

    /**
     * Number of registered validations.
     */
    uint32_t validation_count;

    runtime_manager_t *runtime;
};

static const validation_descriptor_t* validation_system_get_descriptor(validation_system_t *system, const char *name) {

    if(NULL == system || NULL == name) {
        return NULL;
    }
    
    for(uint32_t index = 0; index < system->validation_count; index++) {

        if(safe_string_compare(system->validations[index].name,name)) {

            return system->validations[index].descriptor;
        }
    }
    return NULL;
}

static int32_t validation_system_register_descriptor(validation_system_t *system, const char *name, const validation_descriptor_t *descriptor) {

    if(NULL == system || NULL == name || NULL == descriptor) {
        return -1;
    }
    if(VALIDATION_MAX_COUNT <= system->validation_count) {
        return -1;
    }
    /*
     * Check duplicate validation
     */
    for(uint32_t index = 0; index < system->validation_count; index++) {

        if(safe_string_compare(system->validations[index].name,name)) {
            return -1;
        }
    }
    safe_string_copy(system->validations[system->validation_count].name, name, 
        sizeof(system->validations[system->validation_count].name));
    system->validations[system->validation_count].descriptor = descriptor;
    system->validation_count++;
    return 0;
}

static int32_t validation_system_register_all_descriptors(validation_system_t *system) {

    if(NULL == system) {
        return -1;
    }
    test_manager_t *test_manager = NULL;
    uint32_t test_count = 0;
    test_def_t test_defination = {0};
    runtime_manager_t *runtime = system->runtime;
    
    test_manager = runtime_manager_get_test_manager(runtime);
    if(NULL == test_manager) {
        return -1;
    }

    if(0 != test_manager_get_count(test_manager, &test_count)) {
        return -1;
    }

    for(uint32_t index = 0; index < test_count; index++) {
        if(0 != test_manager_get_test_by_index(test_manager, index, &test_defination)) {
            continue;;
        }
        uint32_t registration_index = 0;
        while (validation_registry[registration_index] != NULL) {
            const validation_descriptor_t *descriptor;

            descriptor = validation_registry[registration_index]();

            if(safe_string_compare(test_defination.name,descriptor->name)) {
                validation_system_register_descriptor(system,test_defination.name,descriptor);
            }
            registration_index++;
        }
    }
    return 0;
}
validation_system_t* validation_system_init(runtime_manager_t *runtime) {

    validation_system_t *system;

    if(NULL == runtime) {
        return NULL;
    }

    system = (validation_system_t *)malloc(sizeof(validation_system_t));

    if(NULL == system) {
        return NULL;
    }

    system->validation_count = 0;
    memset(system->validations, 0, sizeof(system->validations));
    system->runtime = runtime;

    // Initialize the validation system by registering all validations
    validation_system_register_all_descriptors(system);
    return system;
}

int32_t validation_system_deinit(validation_system_t *system) {

    if(NULL == system) {
        return -1;
    }
    free(system);
    return 0;
}

int32_t validation_system_register(validation_system_t *system, const char *name){

    test_manager_t *test_manager = NULL;
    function_manager_t *function_manager = NULL;
    test_def_t test_defination = {0};
    function_def_t function_defination = {0};
    const validation_descriptor_t *descriptor = NULL;
    runtime_manager_t *runtime = system->runtime;

    if(NULL == system || NULL == name) {
        return -1;
    }

    test_manager = runtime_manager_get_test_manager(runtime);
    if(NULL == test_manager) {
        return -1;
    }

    // is the valid test 
    if(0 != test_manager_get_test(test_manager,name,&test_defination)) {
        return -1;
    }

    function_manager = runtime_manager_get_function_manager(runtime);
    if(NULL == function_manager) {
        return -1;
    }

    //check the availiblity of function
    if(0 != function_manager_is_available(function_manager,test_defination.function)) {
        return -1;
    }

    //get the device name associated with function 
    if(0 != function_manager_get_function(function_manager,test_defination.function,&function_defination)) {
        return -1;
    }

    //get test operation for each device supported by function
    /*
     * Get static implementation
     */
    descriptor = validation_system_get_descriptor(system,name);
    if(NULL == descriptor) {
        return -1;
    }

    test_engine_t *test_engine = runtime_manager_get_test_engine(runtime);
    if(NULL == test_engine) {
        return -1;
    }

    /*
     * Register into Test Engine
     */
    return test_engine_register(test_engine, name,descriptor->context,descriptor->ops);
}
int32_t validation_system_register_all(validation_system_t *system) {

    if(NULL == system) {
        return -1;
    }
    test_manager_t *test_manager = NULL;
    uint32_t test_count = 0;
    test_def_t test_defination = {0};
    runtime_manager_t *runtime = system->runtime;
    
    test_manager = runtime_manager_get_test_manager(runtime);
    if(NULL == test_manager) {
        return -1;
    }

    if(0 != test_manager_get_count(test_manager, &test_count)) {
        return -1;
    }

    for(uint32_t index = 0; index < test_count; index++) {

        if(0 != test_manager_get_test_by_index(test_manager, index, &test_defination)) {
            continue;;
        }
        validation_system_register(system, test_defination.name);
    }
    return 0;
}