#ifndef GPIO_DEVICE_H
#define GPIO_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "plugin.h"
#include "gpio-hal.h"

/* Forward declarations for libgpiod types (only on Linux) */
#if defined(__linux__)
struct gpiod_chip;
struct gpiod_line;
#endif

/*----------------------------------------------------------------------------
 * Private Types
 *---------------------------------------------------------------------------*/

/**
 * @brief GPIO device runtime context.
 *
 * One instance is created for every GPIO HAL object.
 *
 * Example:
 *
 * status_led
 * reset_button
 * user_button
 *
 * all have independent runtime contexts.
 */
typedef struct
{
    /**
     * Cached GPIO configuration.
     */
    gpio_configuration_t configuration;

#if defined(__linux__)
        /* libgpiod runtime handles */
        /* per-line chip and line handles to support multiple lines possibly on different chips */
        struct gpiod_chip *chips[GPIO_MAX_LINES];
        struct gpiod_line *lines[GPIO_MAX_LINES];
        uint32_t lines_count;
        /* active_low cached for quick access */
        uint8_t active_low;
#endif

} gpio_device_context_t;

/*----------------------------------------------------------------------------
 * Device Operations
 *---------------------------------------------------------------------------*/

/**
 * @brief Create GPIO runtime object.
 */
int32_t gpio_device_open(
        void *plugin_private_data,
        void **device_private_data);

/**
 * @brief Destroy GPIO runtime object.
 */
int32_t gpio_device_close(
        void *device_private_data);

/**
 * @brief Read GPIO level.
 */
int32_t gpio_device_read(
        void *device_private_data,
        void *buffer,
        uint32_t size);

/**
 * @brief Write GPIO level.
 */
int32_t gpio_device_write(
        void *device_private_data,
        const void *buffer,
        uint32_t size);

/**
 * @brief Generic GPIO control.
 */
int32_t gpio_device_control(
        void *device_private_data,
        uint32_t command,
        void *argument);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_DEVICE_H */