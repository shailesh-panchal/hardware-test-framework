/**
 * @file plugin.h
 *
 * @brief Generic Plugin Interface.
 *
 * This file defines the common interface between
 * Plugin Manager and plugin implementations.
 *
 * A plugin represents a generic capability such as:
 *
 * - GPIO
 * - Camera
 * - I2C
 * - SPI
 * - Storage
 *
 * The framework communicates with plugins only
 * through this interface.
 *
 * Plugin implementation hides platform and HAL
 * details.
 *
 */


#ifndef PLUGIN_H
#define PLUGIN_H


#include <stdint.h>
#include "platform.h"


/**
 * @brief Generic plugin operation interface.
 *
 * Similar concept to Linux file_operations.
 *
 * Each plugin provides implementation of
 * supported operations.
 *
 * Unsupported operations can be NULL.
 */
typedef struct plugin_ops_t
{
    /**
     * @brief Initialize plugin.
     *
     * @param[in] private_data
     * Plugin context.
     *
     * @return status.
     */
    int32_t (*open)(
            void *private_data);

    /**
     * @brief Release plugin.
     *
     * @param[in] private_data
     * Plugin context.
     *
     * @return status.
     */
    int32_t (*close)(
            void *private_data);

    /**
     * @brief Read operation.
     *
     * @param[in] private_data
     * Plugin context.
     *
     * @param[out] buffer
     * Read buffer.
     *
     * @param[in] size
     * Buffer size.
     *
     * @return status.
     */
    int32_t (*read)(
            void *private_data,
            void *buffer,
            uint32_t size);


    /**
     * @brief Write operation.
     *
     * @param[in] private_data
     * Plugin context.
     *
     * @param[in] buffer
     * Write buffer.
     *
     * @param[in] size
     * Buffer size.
     *
     * @return status.
     */
    int32_t (*write)(
            void *private_data,
            const void *buffer,
            uint32_t size);

    /**
     * @brief Generic control operation.
     *
     * Similar to ioctl().
     *
     * Used for device specific commands.
     *
     * Example:
     *
     * GPIO_SET_HIGH
     * CAMERA_START
     *
     * @param[in] private_data
     * Plugin context.
     *
     * @param[in] command
     * Command identifier.
     *
     * @param[in,out] argument
     * Command data.
     *
     * @return status.
     */
    int32_t (*control)(
            void *private_data,
            uint32_t command,
            void *argument);
} plugin_ops_t;

/**
 * @brief Generic plugin object.
 *
 * Every plugin must provide this object.
 *
 * Plugin Manager stores this object.
 */
typedef struct plugin_t
{
    /**
     * Plugin name.
     *
     * Example:
     *
     * gpio0
     * camera0
     */
    char name[PLUGIN_NAME_LENGTH];

    /**
     * Plugin operations.
     */
    plugin_ops_t *ops;

    /**
     * Plugin private data.
     *
     * Owned by plugin implementation.
     *
     * Example:
     *
     * gpio_context_t
     * camera_context_t
     */
    void *private_data;
} plugin_t;

/**
 * @brief Plugin initialization entry.
 *
 * Dynamic plugin (.so) exports this function.
 *
 * Plugin Manager calls this after loading.
 *
 * @param[in] manager
 * Plugin Manager instance.
 *
 * @return status.
 */
typedef int32_t (*plugin_init_t)(
        void *manager);

/**
 * @brief Plugin deinitialization entry.
 *
 * Called before unloading plugin.
 *
 * @return status.
 */
typedef int32_t (*plugin_deinit_t)(
        void);

#endif /* PLUGIN_H */