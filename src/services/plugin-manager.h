/**
 * @file plugin-manager.h
 *
 * @brief Plugin Manager Interface.
 *
 * Plugin Manager maintains all registered
 * plugins and provides plugin discovery
 * and lifecycle management.
 *
 * Plugin Manager does not contain hardware
 * specific knowledge.
 *
 * It only manages generic plugin objects
 * defined by plugin.h.
 *
 */


#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H


#include <stdint.h>
#include <pthread.h>
#include "plugin.h"


#define PLUGIN_MANAGER_MAX_PLUGINS 64

/**
 * @brief Plugin Manager object.
 *
 * Maintains plugin registry.
 */
typedef struct plugin_manager_t plugin_manager_t;

/**
 * @brief Initialize Plugin Manager.
 *
 * Creates plugin registry and
 * synchronization resources.
 *
 * @return Plugin Manager instance.
 */
plugin_manager_t* plugin_manager_init(void);

/**
 * @brief Deinitialize Plugin Manager.
 *
 * Closes all plugins and releases resources.
 *
 * @param[in] manager
 * Plugin Manager instance.
 *
 * @return status.
 */
int32_t plugin_manager_deinit(
        plugin_manager_t *manager);

/**
 * @brief Register plugin.
 *
 * Adds plugin into registry.
 *
 * @param[in] manager
 * Plugin Manager instance.
 *
 * @param[in] plugin
 * Plugin object.
 *
 * @return status.
 */
int32_t plugin_manager_register(
        plugin_manager_t *manager,
        plugin_t *plugin);


/**
 * @brief Get plugin.
 *
 * Finds plugin by name.
 *
 * @param[in] manager
 * Plugin Manager instance.
 *
 * @param[in] name
 * Plugin name.
 *
 * @return Plugin object.
 *
 * @retval NULL
 * Plugin not found.
 */
plugin_t* plugin_manager_get(
        plugin_manager_t *manager,
        const char *name);

/**
 * @brief Open plugin.
 *
 * Calls plugin open operation.
 *
 * @param[in] plugin
 * Plugin object.
 *
 * @return status.
 */
int32_t plugin_manager_open(
        plugin_t *plugin);

/**
 * @brief Close plugin.
 *
 * Calls plugin close operation.
 *
 * @param[in] plugin
 * Plugin object.
 *
 * @return status.
 */
int32_t plugin_manager_close(
        plugin_t *plugin);

/**
 * @brief Open all registered plugins.
 *
 * Called during framework startup.
 *
 * @param[in] manager
 * Plugin Manager instance.
 *
 * @return status.
 */
int32_t plugin_manager_open_all(
        plugin_manager_t *manager);

/**
 * @brief Close all registered plugins.
 *
 * Called during framework shutdown.
 *
 * @param[in] manager
 * Plugin Manager instance.
 *
 * @return status.
 */
int32_t plugin_manager_close_all(
        plugin_manager_t *manager);

#endif /* PLUGIN_MANAGER_H */