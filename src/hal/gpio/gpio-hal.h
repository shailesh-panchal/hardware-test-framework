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
#define GPIO_MAX_LINES 8

typedef struct
{
    char       chip[CONFIG_KEY_LENGTH];
    uint32_t          line;
    /* Support multiple lines per configuration (backwards compatible)
     * - If lines_count == 0, legacy 'line' and 'chip' fields are used.
     * - If lines_count > 0, 'lines' and optional 'lines_chip' are used.
     */
    uint32_t          lines_count;
    uint32_t          lines[GPIO_MAX_LINES];
    char              lines_chip[GPIO_MAX_LINES][CONFIG_KEY_LENGTH];
    gpio_direction_t  direction;
    uint8_t           active_low;
    uint32_t          directions_count;
    gpio_direction_t  directions[GPIO_MAX_LINES];
    uint32_t          active_low_count;
    uint8_t           active_low_lines[GPIO_MAX_LINES];

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
 * @brief Read multiple GPIO levels.
 *
 * @param[in] handle
 * @param[out] levels
 * @param[in] count
 * Number of levels to read.
 *
 * @return Status.
 */
int32_t gpio_hal_read_multi(
        gpio_handle_t *handle,
        gpio_level_t *levels,
        uint32_t count);

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
 * @brief Write multiple GPIO levels.
 *
 * @param[in] handle
 * @param[in] levels
 * @param[in] count
 * Number of levels to write.
 *
 * @return Status.
 */
int32_t gpio_hal_write_multi(
        gpio_handle_t *handle,
        const gpio_level_t *levels,
        uint32_t count);

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