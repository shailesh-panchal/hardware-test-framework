/**
 * @file plugin-manager.c
 *
 * @brief Plugin Manager implementation.
 *
 * Provides plugin registry and lifecycle
 * management.
 */


#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <dlfcn.h>
#endif
#include "logger.h"

#include "plugin-manager.h"
#include "safe_string.h"

/**
 * @brief Plugin Manager object.
 *
 * Maintains plugin registry.
 */
struct plugin_manager_t
{
    /**
     * Registered plugins.
     */
    plugin_t *plugins[PLUGIN_MANAGER_MAX_PLUGINS];

    /**
     * Number of registered plugins.
     */
    uint32_t plugin_count;

    /**
     * Protect plugin registry.
     */
    pthread_mutex_t lock;

};


/**
 * @brief Find plugin index by name.
 */
static int32_t plugin_manager_find(plugin_manager_t *manager, const char *name) {

    for(uint32_t index = 0; index < manager->plugin_count; index++) {

        if(safe_string_compare(manager->plugins[index]->name,name)) {
            return index;
        }
    }
    return -1;
}

plugin_manager_t* plugin_manager_init(void) {

    plugin_manager_t *manager;


    manager =(plugin_manager_t *)malloc(sizeof(plugin_manager_t));
    if(manager == NULL) {
        return NULL;
    }
    pthread_mutex_init(&manager->lock, NULL);
    return manager;
}

int32_t plugin_manager_deinit(plugin_manager_t *manager) {

    if(manager == NULL) {
        return -1;
    }
    pthread_mutex_destroy(&manager->lock);
    free(manager);
    return 0;
}
int32_t plugin_manager_load(plugin_manager_t *manager, plugin_t *plugin, const char *directory) {
    if(manager == NULL || plugin == NULL || directory == NULL) {
        return -1;
    }

    char plugin_path[PLATFORM_PLUGIN_DIRECTORY_LENGTH] = {0};

    safe_sprintf(plugin_path, sizeof(plugin_path), "%s/%s.so", directory, plugin->name);

    pthread_mutex_lock(&manager->lock);

    /*
     * Check duplicate
     */
    if(plugin_manager_find(manager,plugin->name) >= 0) {

        pthread_mutex_unlock(&manager->lock);
        return -1;
    }

    pthread_mutex_unlock(&manager->lock);

#ifndef _WIN32

    plugin->private_data = dlopen(plugin_path, RTLD_NOW);

    if (plugin->private_data == NULL) {
        LOG_ERROR("%s\n", dlerror());
        return -1;
    }

    dlerror();

    plugin->plugin_init = dlsym(plugin->private_data, "plugin_init");
    char *err = dlerror();
    if(err){
        LOG_ERROR("failed to get plugin_init symbol: %s\n", err);

        dlclose(plugin->private_data);
        return -1;
    }
    plugin->plugin_deinit = dlsym(plugin->private_data, "plugin_deinit");
    err = dlerror();
    if(err){
        LOG_ERROR("failed to get plugin_deinit symbol: %s\n", err);

        dlclose(plugin->private_data);
        return -1;
    }
#endif

    plugin->ops = plugin->plugin_init();

    return 0;
}

int32_t plugin_manager_unload(plugin_manager_t *manager, plugin_t *plugin) {
    if(manager == NULL || plugin == NULL) {
        return -1;
    }

    if(plugin->plugin_deinit) {
        plugin->plugin_deinit();
    }
#ifndef _WIN32
    if(plugin->private_data) {
        dlclose(plugin->private_data);
    }
#endif
    return 0;
}

int32_t plugin_manager_register(plugin_manager_t *manager, plugin_t *plugin) {

    if(manager == NULL || plugin == NULL) {
        return -1;
    }

    pthread_mutex_lock(&manager->lock);

    if(manager->plugin_count >= PLUGIN_MANAGER_MAX_PLUGINS) {

        pthread_mutex_unlock(&manager->lock);
        return -1;
    }
    /*
     * Check duplicate
     */
    if(plugin_manager_find(manager,plugin->name) >= 0) {

        pthread_mutex_unlock(&manager->lock);
        return -1;
    }

    manager->plugins[manager->plugin_count] = plugin;
    manager->plugin_count++;
    pthread_mutex_unlock(&manager->lock);
    return 0;
}

plugin_t* plugin_manager_get(plugin_manager_t *manager, const char *name) {

    plugin_t *plugin = NULL;

    if(manager == NULL || name == NULL) {
        return NULL;
    }
    pthread_mutex_lock(&manager->lock);

    int32_t index;

    index = plugin_manager_find(manager,name);

    if(index >= 0) {
        plugin = manager->plugins[index];
    }
    pthread_mutex_unlock(&manager->lock);
    return plugin;
}