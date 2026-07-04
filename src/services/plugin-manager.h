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
 * @brief Load a plugin shared library.
 *
 * Loads the shared library corresponding to the
 * specified plugin, resolves the exported plugin
 * descriptor and registers the plugin with the
 * Plugin Manager.
 *
 * Loading sequence:
 *
 * plugin_manager_load()
 *        │
 *        ▼
 * Construct shared library path
 *        │
 *        ▼
 * dlopen()
 *        │
 *        ▼
 * dlsym("plugin")
 *        │
 *        ▼
 * plugin_manager_register()
 *
 * The plugin must export a global symbol named
 * "plugin" of type plugin_t.
 *
 * If the plugin is already loaded, the function
 * returns success without loading it again.
 *
 * Example:
 *
 * directory:
 *     ./plugins/rk3588
 *
 * plugin->name:
 *     gpio_linux
 *
 * Shared library:
 *     ./plugins/rk3588/libgpio_linux.so
 *
 * @param[in,out] manager
 * Plugin Manager instance.
 *
 * @param[in] plugin
 * Plugin descriptor containing at least the
 * plugin name. On successful return, the
 * descriptor is populated with the exported
 * plugin information.
 *
 * @param[in] directory
 * Directory containing plugin shared libraries.
 *
 * @return Status code.
 *
 * @retval 0
 * Plugin loaded successfully.
 *
 * @retval -1
 * Invalid parameter.
 */
int32_t plugin_manager_load(
        plugin_manager_t *manager,
        plugin_t *plugin,
        const char *directory);

/**
 * @brief Unload a plugin.
 *
 * Unregisters the specified plugin from the Plugin
 * Manager and unloads its associated shared library.
 *
 * Unloading sequence:
 *
 * plugin_manager_unload()
 *        │
 *        ▼
 * Verify plugin is loaded
 *        │
 *        ▼
 * Verify plugin is not in use
 *        │
 *        ▼
 * plugin_manager_unregister()
 *        │
 *        ▼
 * dlclose()
 *
 * A plugin cannot be unloaded while it is being
 * referenced by one or more active test instances.
 *
 * @param[in,out] manager
 * Plugin Manager instance.
 *
 * @param[in] plugin
 * Plugin descriptor to unload.
 *
 * @return Status code.
 *
 * @retval 0
 * Plugin unloaded successfully.
 *
 * @retval -1
 * Invalid parameter.
 */
int32_t plugin_manager_unload(
        plugin_manager_t *manager,
        plugin_t *plugin);

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