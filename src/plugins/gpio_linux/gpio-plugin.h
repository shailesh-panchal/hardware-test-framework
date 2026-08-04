#ifndef GPIO_PLUGIN_H
#define GPIO_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin.h"

/**
 * @brief Plugin entry point.
 *
 * Called once by the Plugin Manager after the shared library
 * is loaded.
 *
 * @return Pointer to plugin operations.
 */
plugin_ops_t *gpio_plugin_init(void);

/**
 * @brief Plugin exit point.
 *
 * Called once by the Plugin Manager before unloading
 * the shared library.
 */
void gpio_plugin_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PLUGIN_H */