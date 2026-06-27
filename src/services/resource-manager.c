/**
 * @file resource-manager.c
 *
 * @brief Resource Manager implementation.
 *
 * Handles resource registration, ownership,
 * availability checking, acquire and release
 * operations for parallel test execution.
 */


#include <stdlib.h>
#include <string.h>

#include "resource-manager.h"
#include "safe_string.h"

/**
 * @brief Resource Manager object.
 *
 * Maintains all registered resources and
 * protects resource ownership information.
 */
struct resource_manager_t
{
    /**
     * Function Manager reference.
     *
     * Used to query required devices
     * for a function.
     */
    function_manager_t *function_manager;
     /**
     * Registered resources.
     */
    resource_info_t resources[RESOURCE_MANAGER_MAX_RESOURCES];

    /**
     * Number of registered resources.
     */
    uint32_t resource_count;

    /**
     * Resource table lock.
     *
     * Protects concurrent access from
     * multiple worker threads.
     */
    pthread_mutex_t lock;

};


/**
 * @brief Find resource index.
 *
 * @param[in] manager
 *        Resource Manager instance.
 *
 * @param[in] name
 *        Resource name.
 *
 * @return Resource index.
 *
 * @retval -1 Not found.
 */
static int32_t resource_manager_find(resource_manager_t *manager, const char *name) {

    for(uint32_t index = 0; index < manager->resource_count; index++) {

        if(safe_string_compare(manager->resources[index].name,name)) {
            return index;
        }
    }
    return -1;
}

resource_manager_t* resource_manager_init(function_manager_t *funciont_manager){

    resource_manager_t *manager = NULL;

    if(funciont_manager == NULL) {
        return NULL;
    }

    manager = (resource_manager_t *)malloc(sizeof(resource_manager_t));
    if(manager == NULL) {
        return NULL;
    }
    pthread_mutex_init(&manager->lock, NULL);
    manager->resource_count = 0;
    manager->function_manager = funciont_manager;
    return manager;
}

int32_t resource_manager_deinit(resource_manager_t *manager) {

    if(manager == NULL) {
        return -1;
    }

    pthread_mutex_destroy(&manager->lock);

    free(manager);

    return 0;
}

int32_t resource_manager_register(resource_manager_t *manager, const char *name) {

    if(manager == NULL || name == NULL) {
        return -1;
    }

    pthread_mutex_lock(&manager->lock);

    if(manager->resource_count >= RESOURCE_MANAGER_MAX_RESOURCES) {

        pthread_mutex_unlock(&manager->lock);
        return -2;
    }
    /*
     * Check duplicate resource
     */
    if(resource_manager_find(manager, name) >= 0) {

        pthread_mutex_unlock(&manager->lock);
        return -1;
    }

    resource_info_t *resource =NULL;


    resource = &manager->resources[manager->resource_count];

    safe_string_copy(resource->name, name, sizeof(resource->name));

    resource->state = RESOURCE_STATE_FREE;
    resource->owner = NULL;
    manager->resource_count++;
    pthread_mutex_unlock(&manager->lock);
    return 0;
}

bool resource_manager_is_available( resource_manager_t *manager, const char *function_name) {

    if(manager == NULL || function_name == NULL) {
        return false;
    }

    bool available = true;


    if(0 == function_manager_is_available(manager->function_manager,function_name)) {
        printf("resource not available with function name %s\n",function_name);
        return false;
    }

    uint32_t device_count = 0;

    if(0 == function_manager_get_device_count(manager->function_manager,function_name, &device_count)) {
        printf("associated device is not available for function name %s\n",function_name);
        return false;
    }

    pthread_mutex_lock(&manager->lock);


    for(uint32_t device_index =0; device_index < device_count; device_index++) {

        device_name_t device_name = {0};
        if(0 != function_manager_get_device_by_index(manager->function_manager,function_name,device_index, &device_name)) {
            continue;
        }
        for(uint32_t index = 0; index < manager->resource_count; index++){

            int32_t resource_index;
            resource_index = resource_manager_find(manager, device_name.name);
            if(resource_index < 0) {
                available = false;
                break;
            }

            if(manager->resources[resource_index].state == RESOURCE_STATE_BUSY) {

                available = false;
                break;
            }
        }
    }
    pthread_mutex_unlock(&manager->lock);
    return available;
}
int32_t resource_manager_acquire(resource_manager_t *manager, const char *function_name, test_instance_t *instance) {

    if(manager == NULL || instance == NULL || function_name == NULL) {
        return -1;
    }

    if(0 == function_manager_is_available(manager->function_manager,function_name)) {
        printf("resource not available with function name %s\n",function_name);
        return false;
    }

    uint32_t device_count = 0;

    if(0 == function_manager_get_device_count(manager->function_manager,function_name, &device_count)) {
        printf("associated device is not available for function name %s\n",function_name);
        return false;
    }

    pthread_mutex_lock(&manager->lock);

    for(uint32_t device_index =0; device_index < device_count; device_index++) {

        device_name_t device_name = {0};
        if(0 != function_manager_get_device_by_index(manager->function_manager,function_name,device_index, &device_name)) {
            continue;
        }

        for(uint32_t index = 0; index < manager->resource_count; index++) {

            int32_t resource_index;
            resource_index = resource_manager_find(manager, device_name.name);

            if(resource_index < 0 || manager->resources[resource_index].state == RESOURCE_STATE_BUSY) {

                pthread_mutex_unlock(&manager->lock);
                return -1;
            }
        }
    }

    for(uint32_t device_index =0; device_index < device_count; device_index++) {

        device_name_t device_name = {0};
        if(0 != function_manager_get_device_by_index(manager->function_manager,function_name,device_index, &device_name)) {
            continue;
        }
        for(uint32_t index = 0; index < manager->resource_count; index++) {

            int32_t resource_index;

            resource_index = resource_manager_find(manager, device_name.name);
            manager->resources[resource_index].state = RESOURCE_STATE_BUSY;

            manager->resources[resource_index].owner = instance;

        }
    }
    pthread_mutex_unlock(&manager->lock);
    return 0;
}

int32_t resource_manager_release(resource_manager_t *manager, const char *function_name, test_instance_t *instance) {

    if(manager == NULL || instance == NULL || function_name == NULL) {
        return -1;
    }
    if(0 == function_manager_is_available(manager->function_manager,function_name)) {
        printf("resource not available with function name %s\n",function_name);
        return false;
    }

    uint32_t device_count = 0;

    if(0 == function_manager_get_device_count(manager->function_manager,function_name, &device_count)) {
        printf("associated device is not available for function name %s\n",function_name);
        return false;
    }

    pthread_mutex_lock(&manager->lock);

    for(uint32_t device_index =0; device_index < device_count; device_index++) {

        device_name_t device_name = {0};
        if(0 != function_manager_get_device_by_index(manager->function_manager,function_name,device_index, &device_name)) {
            continue;
        }

        for(uint32_t index = 0; index < manager->resource_count; index++){

            int32_t resource_index;
            resource_index = resource_manager_find(manager, device_name.name);
            if(resource_index >= 0) {

                if(manager->resources[resource_index].owner == instance) {

                    manager->resources[resource_index].state = RESOURCE_STATE_FREE;

                    manager->resources[resource_index].owner = NULL;
                }
            }
        }
    }
    pthread_mutex_unlock(&manager->lock);
    return 0;
}