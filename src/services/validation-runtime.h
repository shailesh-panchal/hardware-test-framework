#ifndef VALIDATION_RUNTIME_H
#define VALIDATION_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio-hal.h"
#include "runtime.h"

/**
 * @brief Initialize validation runtime.
 *
 * Called once by runtime_init().
 *
 * @return Status.
 */
int32_t validation_runtime_init(runtime_manager_t *runtime);

/**
 * @brief Deinitialize validation runtime.
 *
 * @return Status.
 */
int32_t validation_runtime_deinit(void);

/**
 * @brief Open GPIO device.
 *
 * @param[in] device_name
 * Device name from function.json/platform.json.
 *
 * Example:
 *
 * status_led
 * reset_button
 *
 * @return GPIO handle.
 */
gpio_handle_t *validation_runtime_open_gpio(
        const char *device_name,
        gpio_configuration_t *configuration);

/**
 * @brief Close GPIO device.
 *
 * @param[in] handle
 *
 * @return Status.
 */
int32_t validation_runtime_close_gpio(
        gpio_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif