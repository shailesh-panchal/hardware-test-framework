#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "plugin.h"

/*----------------------------------------------------------------------------
 * Enumerations
 *---------------------------------------------------------------------------*/

/**
 * @brief GPIO direction.
 */
typedef enum
{
    GPIO_DIRECTION_INPUT = 0,
    GPIO_DIRECTION_OUTPUT
} gpio_direction_t;

/**
 * @brief GPIO logic level.
 */
typedef enum
{
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH
} gpio_level_t;

/**
 * @brief GPIO control commands.
 */
typedef enum
{
    GPIO_COMMAND_SET_CONFIGURATION = 0,
    GPIO_COMMAND_SET_DIRECTION,
    GPIO_COMMAND_GET_DIRECTION,
    GPIO_COMMAND_SET_ACTIVE_LOW,
    GPIO_COMMAND_GET_ACTIVE_LOW
} gpio_command_t;

/*----------------------------------------------------------------------------
 * Data Types
 *---------------------------------------------------------------------------*/

/**
 * @brief GPIO configuration.
 *
 * Filled by Platform Manager from platform.json.
 */
typedef struct
{
    char       chip[CONFIG_KEY_LENGTH];
    uint32_t          line;
    gpio_direction_t  direction;
    uint8_t           active_low;

} gpio_configuration_t;

/**
 * @brief Opaque GPIO handle.
 */
typedef struct gpio_handle gpio_handle_t;

/*----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

/**
 * @brief Create GPIO HAL object.
 *
 * @param[in] plugin
 * Loaded GPIO plugin.
 *
 * @return GPIO handle.
 */
gpio_handle_t *gpio_hal_open(
        plugin_t *plugin);

/**
 * @brief Destroy GPIO HAL object.
 *
 * @param[in] handle
 * GPIO handle.
 *
 * @return Status.
 */
int32_t gpio_hal_close(
        gpio_handle_t *handle);

/**
 * @brief Configure GPIO.
 *
 * @param[in] handle
 * GPIO handle.
 *
 * @param[in] configuration
 * GPIO configuration.
 *
 * @return Status.
 */
int32_t gpio_hal_configure(
        gpio_handle_t *handle,
        const gpio_configuration_t *configuration);

/**
 * @brief Read GPIO level.
 *
 * @param[in] handle
 *
 * @param[out] level
 *
 * @return Status.
 */
int32_t gpio_hal_read(
        gpio_handle_t *handle,
        gpio_level_t *level);

/**
 * @brief Write GPIO level.
 *
 * @param[in] handle
 *
 * @param[in] level
 *
 * @return Status.
 */
int32_t gpio_hal_write(
        gpio_handle_t *handle,
        gpio_level_t level);

/**
 * @brief Generic GPIO control.
 *
 * @param[in] handle
 *
 * @param[in] command
 *
 * @param[in,out] argument
 *
 * @return Status.
 */
int32_t gpio_hal_control(
        gpio_handle_t *handle,
        gpio_command_t command,
        void *argument);

#ifdef __cplusplus
}
#endif

#endif